#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char const *argv[])
{
	if(argc !=4)
		printf("Command line argument incorrect\n");
	else
	{
		int a = atoi(argv[1]);
		int b = atoi(argv[3]);
		if(strcmp(argv[2],"+")==0)
			printf("%d\n",a+b);
		else if(strcmp(argv[2],"-")==0)
			printf("%d\n",a-b);
		else if(strcmp(argv[2],"*")==0)
			printf("%d\n",a*b);
		else if(strcmp(argv[2],"/")==0)
			printf("%d\n",a/b);
		else printf("Invalid Command\n");
	}
	return 0;
}