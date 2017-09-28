#include <stdio.h>
#include <unistd.h>
int main()
{
	for(int i=0;i<5;i++)
	{
		fork();
	}
	printf("Exiting Process id: %d\n",getpid());
	return 0;
}