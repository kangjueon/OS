/*
 2023.03.13(mon) process genearte
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void)
{
    int pid, ppid;
    int x = 0;
    
    /*
     * 프로그램 시작 지점에서 x 값을 출력해본다.
     */
    printf("시작 x = %d\n", x); 
    /*
     * 현재 프로세스의 ID 값을 읽고 출력한다.
     */
    printf("프로세스 ID = %d\n", ppid = getpid());
    /*
     * fork()를 사용하여 현재 프로세스를 복제한다.
     */
    if ((pid = fork()) > 0) {   /* 부모 프로세스 */
        printf("부모 프로세스: 본인 ID = %d, 자식 ID = %d\n", getpid(), pid);

	wait(NULL);
        for (int i = 0; i < 5; ++i) {
            printf("하!하!하!\n");
            ++x;
        }
        printf("부모 x = %d\n", x);
    }
    else if (pid == 0) {        /* 자식 프로세스 */
        printf("자식 프로세스: 본인 ID = %d, 부모 ID = %d\n", getpid(), ppid);
        for (int i = 0; i < 5; ++i) {
            printf("호~호~호~\n");

            --x;   //does not depend on parents' parameter x
        }
        printf("자식 x = %d\n", x);
    }
    else {                      /* 프로세스 생성 오류 발생 */
        printf("fork() error!\n");
        return 1;
    }
    /*
     * 아래 코드는 부모와 자식이 모두 실행한다. 왜 그럴까?
     * 변수 x를 부모와 자식이 공유하고 있나, 아니면 따로 가지고 있나?
     */
    printf("종료 x = %d\n", x);
    printf("딩동댕~♬\n");
    return 0;
}
