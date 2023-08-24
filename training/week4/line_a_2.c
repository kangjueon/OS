#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define	BUFFER_SIZE	25
#define	READ_END	0
#define	WRITE_END	1
int value = 5;
int main(void) {
	int len;
	int fdA[2];   // 부모 -> 자식으로 메세지를 보내는 파이프 
	int fdB[2];   // 자식 -> 부모로 메세지를 보내는 파이프
	char buf[BUFFER_SIZE];
	char read_msg[BUFFER_SIZE];
	pid_t pid; 
		
	
	
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
   
	if (pid == 0) { //child
		value += 15;   
		sprintf(buf, "%d", value);  // int형 value를 char형buf에 넣
		close(fdA[0]);              //안쓰는 파이프 닫
		write(fdB[1], buf, strlen(buf)+1);//  buf값 부모 프로세스의 입구에 저장.
		close(fdB[1]);
		
		
		
	} else {  //parent process
		
		read(fdB[0], read_msg,BUFFER_SIZE) ;// 부모 프로세스의 입구에 저장된 값을 읽는다(자식으로부터 받음)
		printf("parent process value : %s\n",read_msg );
		
		
		
	}
	
	
	
		
	
	return 0;
}
