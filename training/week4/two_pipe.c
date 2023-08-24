#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#define	BUFFER_SIZE	25
#define	READ_END	0
#define	WRITE_END	1

int main(void) {
	char write_msg[BUFFER_SIZE];
	char read_msg[BUFFER_SIZE];
	char buf[BUFFER_SIZE];
	int fdA[2];   // 부모 -> 자식으로 메세지를 보내는 파이프 
	int fdB[2];   // 자식 -> 부모로 메세지를 보내는 파이프
	pid_t pid; 
	int count1=0, count2=10000 , tmp1, tmp2;
	
	printf("\n\n");

	if (pipe(fdA) == -1) {
		printf("PIPE ERROR\n");
		fprintf(stderr, "PipeA failed");
		return 1;
	}
	
	if (pipe(fdB) == -1) {
		printf("PIPE ERROR\n");
		fprintf(stderr, "PipeB failed");
		return 1;
	}	
	pid = fork();

	if (pid < 0) {
		printf("FORK ERROR\n");
		fprintf(stderr, "Fork failed");
		return 1;
	}
   
	if(pid>0){ //parent process
                close(fdA[0]);
                close(fdB[1]);
                for(int i=0; i< 5; i++){
                        sprintf(buf,"parent %d",count1++);//char형buf에 int형 인자를 넣는 함수
                        write(fdA[1],buf,BUFFER_SIZE);
                        memset(buf,0,sizeof(buf));
                        read(fdB[0],buf,BUFFER_SIZE);
                        printf("parent got message : %s\n",buf);
                        sleep(1);
                }
        }else{  //child process
                close(fdA[1]);
                close(fdB[0]);
                
               for(int i = 0 ; i < 5; i++){
                        sprintf(buf,"child %d",count2++);
                        write(fdB[1],buf,BUFFER_SIZE);
                        memset(buf,0,sizeof(buf));
                        read(fdA[0],buf,BUFFER_SIZE);
                        printf("child got message : %s\n",buf);
                        sleep(1);
                }
        }
	return 0;
}
