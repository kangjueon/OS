#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t child;
  long orig_rax, rax;
  long params[3];
  int status;
  int insyscall = 0;
  struct user_regs_struct regs;
  child = fork();
  if (child == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("/bin/ls", "ls", NULL);
  } else {
    while (1) {
      wait(&status);
      if (WIFEXITED(status))
        break;
      orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
      if (orig_rax == SYS_write) {  // write함수라면?
        if (insyscall == 0) {  // flag로 사용된다. 맞다 or 아니다 확인용. 실행전 샐행후 두번 체크. 실행 전에 체크하는지 후에 체크하는지 확인하기 위함.
          insyscall = 1;       //insysacll = 0이면 실행 전.
          ptrace(PTRACE_GETREGS, child, NULL, &regs);  //변수안의 resgistor값을 모두 읽어옴.
          printf("Write called with "
                 "%lld %lld %lld\n",
                 regs.rdi, regs.rsi, regs.rdx);
        } else {   //실행된 후.
          rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
          printf("Write returned "
                 "with %ld\n",
                 rax);
          insyscall = 0;
        }
      }
      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    }
  }
  return 0;
}

