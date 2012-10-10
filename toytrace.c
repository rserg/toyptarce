#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <linux/ptrace.h>
#include <assert.h>


int do_parent(pid_t pid)
{
 int status;
 //printf("SSS");
    if(waitpid(pid,&status, 0) < 0)
    {
        perror("waitpid");
    }
    if(WIFSTOPPED(status))
        printf("Child process is topped");
    ptrace(PTRACE_SETOPTIONS,pid,0,PTRACE_O_TRACESYSGOOD);
#if defined(unix)
    //wait(&status);
#endif
//TODO support FreeBSD in the future
#if defined(BSD)
    wait3(pid, &status, WNOHANG);
#endif

    while(!WIFEXITED(status))
    {
        ptrace(PTRACE_SYSCALL,pid,0,0);
        waitpid(pid, &status,0);
        if(WIFSTOPPED(status) && WSTOPSIG(status)){
            ptrace(PTRACE_GETREGS,pid,0);

        //assert(!WIFSIGNALED(status));
        if(WIFSIGNALED(status))
        {
            printf("killed by %d", WCOREDUMP(status));
            ptrace(PTRACE_GETREGS, pid, 0);
        }

        ptrace(PTRACE_SYSCALL,pid,0,0);
        waitpid(pid, &status, 0);
        }



            //ptrace(PTRACE_POKETEXT,pid,(void*)(text+7),0x72626168);
         //i   return 1;
    }

}

void do_child()
{
  ptrace(PTRACE_TRACEME, 0,0,0);
  execl("/bin/ls", "ls",NULL);
  kill(getpid(),SIGSTOP);
  perror("execl");
}


pid_t createfork()
{
    pid_t pid;
    switch(pid = fork())
    {
        case -1:
            perror("stop");
            break;
        case 0:
           ptrace(PTRACE_TRACEME,0,0,0);
           break;
       // default:
       //`    par(pid);
    }

    return pid;
}

void main()
{
 pid_t pid;

 pid = fork();
 if(pid == 0)
    do_child();
 else
     do_parent(pid);
}
 //pid = getpid();

 //lkml.org}
