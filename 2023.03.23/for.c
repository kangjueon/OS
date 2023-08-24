/*
 * Copyright 2022-2023. Heekuck Oh, all rights reserved.
 *
 * 이 프로그램은 한양대학교 ERICA 컴퓨터학부 재학생을 위한 omp parallel for 예제이다.
 * 시스템 코어의 개수 만큼 스레드를 생성하고 for 루프를 스레드에 분할하여 할당한다.
 * 각 스레드는 서로 다른 코어에서 실행되므로 병렬처리의 효과를 얻을 수 있다.
 * for 루프의 어느 부분이 스레드에 할당되는지 관찰한다. 
 */
#include <stdio.h>
#include <omp.h>

#define N 19

int main(void)
{
    int i;
    
    printf("forking...\n");
    #pragma omp parallel for
    for (i = 0; i < N; ++i) {
        int id = omp_get_thread_num();
        printf("[T%d] i = %d\n", id, i);
    }
    printf("...joined\n");

    printf("forking...\n");
    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < N; ++i) {
        int id = omp_get_thread_num();
        printf("[T%d] i = %d\n", id, i);
    }
    printf("...joined\n");
    
    return 0;
}
