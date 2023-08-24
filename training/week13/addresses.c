#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096 // Page size in bytes
#define ADDRESS_SIZE 32 // Virtual address size in bits

int main(int argc, char *argv[]) {
    /**
     *  프로그램을 실행시 2개이상의 커맨드라인이 필요하다. 따라서 argc < 2인 경우는 1을 return.
    */
    if (argc < 2) {
        printf("Usage: %s [virtual address]\n", argv[0]);
        return 1;
    } 
    /**
     *  ./addresses 19986을 입력하면 argv[1] = 19986을 의미. 
     *  argv[1]을 long long type으로 변환하여 address에 저장한다.
     * address가 (1ULL<< ADDRESS_SIZE) 보다 큰경우, 즉 유효가상범위를 넘었을때 오류를 리턴
    */
    unsigned long long address = atoll(argv[1]);
    if (address >= (1ULL << ADDRESS_SIZE)) {
        printf("Error: Virtual address out of range.\n");
        return 1;
    }

    unsigned int pageNumber, offset;
    // pageNumber(몫) + offset(나머지)
    pageNumber = address / PAGE_SIZE; // Page number calculation 몫
    offset = address % PAGE_SIZE; // Offset calculation 나머지.

    printf("Page number: %u\n", pageNumber);
    printf("Offset: %u\n", offset);

    return 0;
}
