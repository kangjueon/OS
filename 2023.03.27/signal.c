/*
 * Copyright 2022-2023. Heekuck Oh, all rights reserved.
 * 이 프로그램은 한양대학교 ERICA 컴퓨터학부 재학생을 위해 작성하였다.
 * 복잡함을 피하기 위해 시스템 호출의 오류는 검사하지 않았다.
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define N 4
pthread_t tid[N+1];

/*
 * 시그널을 처리하기 위한 사용자 함수로 프로세스 내의 스레드가 공유한다.
 */
void handler(int sig)
{
    pthread_t id;
    int i;
    
    /*
     * 시그널을 수신한 스레드가 누구인지 확인하기 위해 실행중인 스레드의 id를 읽어온다.
     */
    id = pthread_self();
    /*
     * 이 id와 일치하는 것을 스레드 id 테이블에서 찾은 다음 색인 값과 시그널 값을 출력한다.
     */
    for (i = 0; i <= N; ++i)
        if (pthread_equal(id, tid[i])) {
            printf("T%d caught signal %d\n", i, sig);
            break;
        }
}

/*
 * 스레드로 실행할 foo 함수는 시그널이 올 때까지 멈춤 상태로 기다린다.
 * 시그널을 사용자 처리기로 잡을 수 있는 경우에는 처리기가 실행되고 pause()로부터 리턴된다.
 * 잡을 수 없는 경우에는 시스템 기본 처리기가 실행되므로 pause()로부터 돌아오지 않을 수 있다.
 */
void *foo(void *arg)
{
    int id;

    /*
     * 몇 번째 스레드인지 식별하기 위해 인자 값을 읽고 출력한다.
     */
    id = *(int *)arg;
    printf("T%d has been started\n", id);
    /*
     * 시그널이 올 때까지 멈춤 상태로 기다린다. 시그널을 사용자 처리기로 잡을 수 있는 경우에는
     * 처리기가 실행되고 pause()로부터 리턴된 뒤 while 루프를 반복한다.
     * 그렇지 않은 경우에는 시스템 기본 처리기가 실행되므로 다시 돌아오지 않을 수 있다.
     */
    while (1) {
        pause();
        printf("T%d is resumed\n", id);
    }
    /*
     * 아래는 도달하지 않는 코드이지만 일관성을 위해 작성했다.
     */
    pthread_exit(NULL);
}

/*
 * 시그널을 테스트하기 위한 메인 함수
 */
int main(void)
{
    int i, num[N+1];
    struct sigaction act;

    /*
     * 시그널 SIGUSR1, SIGUSR2를 잡아서 처리하기 위한 함수를 설정한다.
     * sa_mask는 시그널 처리기를 수행하는 동안 추가로 차단할 시그널의 집합을 의미하다.
     * 참고로 시그널 처리기가 시그널을 처리하는 동안 같은 시그널은 차단된다.
     */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    /*
     * 메인 스레드의 ID를 읽어둔다.
     */
    tid[0] = pthread_self();
    /*
     * N 개의 자식 스레드를 생성한다.
     */
    for (i = 1; i <= N; ++i) {
        num[i] = i;
        pthread_create(tid+i, NULL, foo, num+i);
    }
    /*
     * 스레드가 실행된지 시간이 지난 효과를 얻기 위해 잠시 쉰다.
     */
    sleep(1); printf("---\n");
    /*
     * N번 스레드에게 SIGUSR1 시그널을 N번 보낸다.
     * 중복된 시그널이 무시되는 현상을 관찰한다.
     */
    for (i = 0; i <= N; ++i)
        pthread_kill(tid[N], SIGUSR1);
    sleep(1); printf("---\n");
    /*
     * 메인 스레드를 포함해서 모든 스레드에게 SIGUSR1, SIGUS2 시그널을 보낸다.
     */
    for (i = 0; i <= N; ++i)
        pthread_kill(tid[i], SIGUSR1);
    for (i = 0; i <= N; ++i)
        pthread_kill(tid[i], SIGUSR2);
    sleep(1); printf("---\n");
    /*
     * 프로세스(메인 스레드)에게도 시그널을 보낸다.
     */
    kill(getpid(), SIGUSR1);
    kill(getpid(), SIGUSR2);
    sleep(1); printf("---\n");
    /*
     * 이번에는 시그널 SIGUSR1, SIGUSR2를 무시하도록 설정한다.
     */
    act.sa_handler = SIG_IGN;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    /*
     * 메인 스레드를 포함해서 모든 스레드에게 SIGUSR1, SIGUS2 시그널을 보낸다.
     * 그러나 두 시그널을 무시하도록 설정되어 있어서 아무 일도 일어나지 않는다.
     */
    for (i = 0; i <= N; ++i)
        pthread_kill(tid[i], SIGUSR1);
    for (i = 0; i <= N; ++i)
        pthread_kill(tid[i], SIGUSR2);
    sleep(1); printf("---\n");
   /*
     * 이번에는 시그널 SIGUSR1, SIGUSR2를 시스템 기본설정으로 되돌린다.
     */
    act.sa_handler = SIG_DFL;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    /*
     * N번 스레드에게 시그널 SIGUSR1을 보낸다.
     * 시그널 SIGUSR1에 대한 시스템 기본설정은 프로세스를 종료하는 것이다.
     */
    pthread_kill(tid[N], SIGUSR1);
    /*
     * 메인 스레드는 자식 스레드와의 조인을 시도한다.
     * 그러나 프로세스가 종료되기 때문에 조인을 시도하지도 못하거나 실패한다.
     */
    for (i = 1; i <= N; ++i)
        pthread_join(tid[i], NULL);
    /*
     * 앞 단계에서 프로세스가 종료되기 때문에 다음 출력을 볼 수 없어야 한다.
     */
    printf("Do you see this?\n");

    return 0;
}
