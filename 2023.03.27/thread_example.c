/*
 * Copyright(c) 2023 All rights reserved by Heekuck Oh.
 * 이 프로그램은 한양대학교 ERICA 컴퓨터학부 학생을 위한 교육용으로 제작되었다.
 * 한양대학교 ERICA 학생이 아닌 이는 프로그램을 수정하거나 배포할 수 없다.
 * 프로그램을 수정할 경우 날짜, 학과, 학번, 이름, 수정 내용을 기록한다.
 */
#include <stdio.h>
#include <pthread.h>

/*
 * 전역변수 x를 모든 스레스가 공유할 수 있도록 앞에 배치하였다.
 */
int x = 0;

/*
 * 함수 foo()는 스레드를 생성하여 실행한다.
 */
void *foo(void *arg)
{
    /*
     * 이 스레드는 호호호 웃으면서 공유변수 x를 감소시킨다.
     */
    printf("서브 스레드\n");
    for (int i = 0; i < 5; ++i) {
        printf("호~호~호~\n");
        --x;
    }
    printf("서브 x = %d\n", x);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;
    
    /*
     * 프로그램 시작 지점에서 공유변수 x 값을 출력해본다.
     */
    printf("시작 x = %d\n", x);
    /*
     * 서브 스레드를 생성하여 함수 foo()를 실행한다.
     * fork()와 달리 프로세스 내의 모든 스레드는 코드와 데이터를 공유한다.
     */
    pthread_create(&tid, NULL, foo, NULL);
    /*
     * 메인 스레드는 하하하 웃으면서 공유변수 x를 증가시킨다.
     */
    printf("메인 스레드\n");
    for (int i = 0; i < 5; ++i) {
        printf("하!하!하!\n");
        ++x;
    }
    printf("메인 x = %d\n", x);
    /*
     * 서브 스레드가 끝날 때까지 기다린다.
     * 만일 메인 스레드가 서브 스레드를 기다리지 않고 먼저 종료해버리면 어떻게 될까?
     */
    pthread_join(tid, NULL);
    /*
     * 공유변수 x를 마지막으로 출력하고 종료한다.
     */
    printf("종료 x = %d\n", x);
    printf("딩동댕~♬\n");
    return 0;
}
