#include <stdio.h>

#define MAX_FRAMES 3

int faults = 0;



int main(void) {
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1 ,7 ,0 ,1};
    int n = sizeof(referenceString) / sizeof(referenceString[0]);
    printf("%d\n", n);
    
    int memory[MAX_FRAMES];
    int frameCount = 0;
    
    for (int i = 0; i < n; i++) {
        int page = referenceString[i];
        int found = 0;
        
        // Check if page is already in memory
        for (int j = 0; j < frameCount; j++) {
            if ( memory[j] == page) {
                found = 1;
                break;
            }
        }
        
        // Page fault occurred
        if (!found) {
            // memory가 비어있다면 
            if (frameCount < MAX_FRAMES) {
                memory[frameCount] = page;  // memory append
                frameCount++;
                faults++;
            } else {  //꽉 찼다면
                // Replace the oldest page (FIFO)
                //memory를 한칸 씩 밀고(pop) 새로운 index를 push.
                for (int j = 0; j < frameCount - 1; j++) {
                    memory[j] = memory[j + 1];
                }
                memory[frameCount - 1] = page;  //memory append
                faults++;
            }
        }

        if (!found) {
            printf("[ ");
            for (int i = 0; i < frameCount; i++) {
                printf("%d ", memory[i]);
            }
            printf("]\n");
        }

         
    }
    printf("Total page faults: %d\n", faults); // 
    return 0;
}
