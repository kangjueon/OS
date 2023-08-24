/*
 * Copyright 2022-2023. Heekuck Oh, all rights reserved.
 *
 * 이 프로그램은 한양대학교 ERICA 컴퓨터학부 재학생을 위한 omp parallel 예제이다.
 * 시스템 코어의 개수 만큼 스레드를 생성하여 중괄호 안에 있는 코드를 실행한다.
 * 각 스레드는 서로 다른 코어에서 실행되므로 병렬처리의 효과를 얻을 수 있다.
 */
#include <stdio.h>
#include <omp.h>

int main(void)
{
    printf("forking...\n");
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("[T%d] Hello, world!\n", id);
    }
    printf("...joined\n");

    printf("forking...\n");
    #pragma omp parallel num_threads(13)
    {
        int id = omp_get_thread_num();
        printf("[T%d] Hello, world!\n", id);
    }
    printf("...joined\n");
    
    return 0;
}
