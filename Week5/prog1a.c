#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>
struct hostent *he;
struct in_addr ipv4_addr;
int main(int argc,char *argv[])
{
	if(argc<2)
	{
		printf("Enter an IP address\n");
		return 0;
	}
	socklen_t len = sizeof(ipv4_addr);

	inet_pton(AF_INET,argv[1],&ipv4_addr);

	he = gethostbyaddr(&ipv4_addr, sizeof(ipv4_addr) , AF_INET);
	if(he ==NULL)
	{
		printf("no idea\n");
		return 0;
	}
	printf("Hostname for above IP is: %s\n",he->h_name);
	return 0;
}