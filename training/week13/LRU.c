#include <stdio.h>

#define MAX_FRAMES 3

int faults_LRU = 0;

int isDuplicate(int memory[], int frameCount, int page) {
    for (int i = 0; i < frameCount; i++) {
        if (memory[i] == page) {
            return 1; // 중복된 페이지가 있는 경우 1을 반환
        }
    }
    return 0; // 중복된 페이지가 없는 경우 0을 반환
}

int findLRUIndex(int counter[], int frameCount) {
    int minCounter = counter[0];
    int minIndex = 0;
    for (int i = 1; i < frameCount; i++) {
        if (counter[i] < minCounter) {
            minCounter = counter[i];
            minIndex = i;
        }
    }
    return minIndex;
}

void updateCounter(int counter[], int frameCount, int index) {
    for (int i = 0; i < frameCount; i++) {
        if (i == index) {
            counter[i] = 0; // 가장 최근에 사용한 페이지의 카운터를 0으로 설정
        } else {
            counter[i]++; // 다른 페이지의 카운터를 1씩 증가
        }
    }
}

int main() {
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1 ,7 ,0 ,1};
    int n = sizeof(referenceString) / sizeof(referenceString[0]);
    
    int memory[MAX_FRAMES];
    int frameCount = 0;
    
    int counter[MAX_FRAMES] = {0}; // 페이지 카운터 배열 초기화
    
    for (int i = 0; i < n; i++) {
        int page = referenceString[i];
        int found = isDuplicate(memory, frameCount, page);
        
        // Page fault occurred
        if (!found) {
            // Memory is not full
            if (frameCount < MAX_FRAMES) {
                memory[frameCount] = page;
                frameCount++;
                faults_LRU++;
            } else { // Memory is full
                int index = findLRUIndex(counter, frameCount);
                memory[index] = page;
                faults_LRU++;
            }
        }
        
        updateCounter(counter, frameCount, found ? -1 : frameCount - 1);
        
        printf("[ ");
        for (int i = 0; i < frameCount; i++) {
            printf("%d ", memory[i]);
        }
        printf("]\n");
    }
    
    printf("Total page faults (LRU): %d\n", faults_LRU);
    return 0;
}
