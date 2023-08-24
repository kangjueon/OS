#include <stdio.h>
#include <stdbool.h>

#define MAX_FRAMES 3

bool search(int key, int memory[], int size) {
    for (int i = 0; i < size; i++) {
        //key과 memory를 비교하여 같다면, 중복임
        if (memory[i] == key) {
            return true;
        }
    }
    return false;
}
/**
 *  가장 먼 미래에 참조될 페이지를 찾는다.
*/
int predict(int pg[], int memory[], int n, int index, int size) {
    int res = -1;  //res = -1이면, 앞으로 참조할 것이라는 뜻.
    int farthest = index;
    for (int i = 0; i < size; i++) {
        int j;
        for (j = index; j < n; j++) {
            if (memory[i] == pg[j]) {
                if (j > farthest) {
                    farthest = j;
                    res = i;
                }
                break;
            }
        }
        //앞으로 참조되지 않음.
        if (j == n) {  
            return i;
        }
    }
    return (res == -1) ? 0 : res;
}



int main(void) {
    int pg[] = { 7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1 ,7 ,0 ,1};
    int n = sizeof(pg) / sizeof(pg[0]);  // 20
    
    int memory[MAX_FRAMES] = {-1}; // -1로 초기화
    int frame_count = 0; // 현재 프레임 수
    
    for (int i = 0; i < n; i++) {
        /*
         * 참조된 페이지가 메모리에 적재되어 있는지(중복되어있는지) 검사
         * search함수는 중복된 페이지를 찾는다.
        */
        if (search(pg[i], memory, frame_count)) {
            
            continue;
        }

        if (frame_count < MAX_FRAMES) {  //memory가 MAX_FRAMES로 다 채워지지 않으면,그냥 채워 넣는다.
            memory[frame_count] = pg[i];
            frame_count++; // 프레임 수 증가
        } else {  //predict함수를 이용하여 가장 먼 미래에 참조될 것을 j에 저장.
            int j = predict(pg, memory, n, i + 1, frame_count);
            memory[j] = pg[i];
        }

        // memory를 출력
        printf("[ ");
        for (int i = 0; i < frame_count; i++) {
           printf("%d ", memory[i]);
        }
        printf("]\n");
    }
    

    return 0;
}
