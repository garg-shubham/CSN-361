#include <stdio.h>
#include <unistd.h>
int main()
{
	pid_t pid,ppid;
	pid = fork();
	if(pid==0)
		printf("Child Process id: %d\n",getpid());
	else
		printf("Parent Process id: %d\n",getpid());
	return 0;
}