/*
 * Copyright 2022-2023. Heekuck Oh, all rights reserved.
 *
 * 이 프로그램은 한양대학교 ERICA 컴퓨터학부 재학생을 위한 TLS 예제이다.
 * counter와 global은 전역변수이므로 모든 스레드에서 보인다. 즉, 공유한다.
 * 반면에 local은 지역변수이므로 해당 스레드에게만 보인다.
 * 우리는 전역변수 global이 해당 스레드마다 별도의 사본을 가질 수 있게 할 수 없을까?
 * 다시 말해 전역변수 global을 마치 지역변수처럼 해당 스레드만 볼 수 있게 할 수 없을까?
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N 8

int counter = 0;
int global;
//__thread int global;
//_Thread_local int global;

void *foo(void *arg)
{
    int local;
    
    global = local = counter++;
    printf("%d--created\n", local);
    
    while (local == global)
        pthread_testcancel();
    
    printf("<%d,%d> %d--finished\n", local, global, local);
    pthread_exit(NULL);
}

int main(void)
{
    int i;
    pthread_t tid[N];
    
    for (i = 0; i < N; ++i)
        pthread_create(tid+i, NULL, foo, NULL);
    
    sleep(1);
    
    for (i = 0; i < N; ++i)
        pthread_cancel(tid[i]);
    for (i = 0; i < N; ++i)
        pthread_join(tid[i], NULL);

    return 0;
}
