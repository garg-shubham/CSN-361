#include <bits/stdc++.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <errno.h>

int datalen = 56;
void sig_alrm(int signo);
void send_v4(void);
void proc_v4(char *ptr, ssize_t len, struct timeval *tvrecv, char *host);
void readloop(char *host);
void tv_sub(struct timeval *recv, struct timeval *send);
short in_cksum(u_short *addr, int len);
using namespace std;

sockaddr_in sasend,sarecv;
struct icmp *icmpx;
int pid;
int nsent;

struct timeval *tvsend,tvx;
int errno;
int sockfd;
/* data that goes with ICMP echo request */
int main(int argc, char **argv)
{
	char* host = argv[1];
	pid = getpid();
	signal(SIGALRM, sig_alrm);
	printf("PING %s : %d data bytes\n", argv[1], datalen);
	bzero(&sasend, sizeof(sasend));
	sasend.sin_family = AF_INET;
	inet_aton(argv[1],(struct in_addr *) &sasend.sin_addr);
	
	readloop(argv[1]);
	exit(0);
}
void sig_alrm(int signo)
{
	send_v4();
	alarm(1);
	return;
	/* probably interrupts recvfrom() */
}
void readloop(char *host)
{
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	setuid(getuid());		/* don't need special permissions any more */
	sig_alrm(SIGALRM);		/* send first packet */
	char recvbuf[4096];
	struct timeval tval;
	for (;;)
	{
		unsigned int size = sizeof(sarecv);
		int n = recvfrom(sockfd, recvbuf, 512, 0,(struct sockaddr*) &sarecv, &size);
		if (n < 0)
		{
			if (errno == EINTR) continue;
			else printf("recvfrom error");
		}
		gettimeofday(&tval, NULL);
		proc_v4(recvbuf, n, &tval, host);
	}
}
void send_v4(void)
{
	struct icmp *icmp_temp;
	int len;
	char sendbuf[4096];
	
	icmpx = (struct icmp *) sendbuf;
	icmpx->icmp_type = ICMP_ECHO;
	icmpx->icmp_code = 0;
	icmpx->icmp_id = pid;
	icmpx->icmp_seq = nsent++;
	gettimeofday((struct timeval *) icmpx->icmp_data, NULL);
	char * msg = "Hello\0";
	strcpy(((char *)icmpx->icmp_data) + sizeof(struct timeval), msg);
	len = 8 + datalen;
	/* checksum ICMP header and data */
	icmpx->icmp_cksum = 0;
	icmpx->icmp_cksum = in_cksum((u_short *) icmpx, len);
	// printf("sent packet %d\n", nsent);
	sendto(sockfd, sendbuf, len, 0, (struct sockaddr *) &sasend, sizeof(sasend));
}
void proc_v4(char *ptr, ssize_t len, struct timeval *tvrecv, char *host)
{
	struct ip *ip_packet = (struct ip *) ptr;		/* start of IP header */
	int hlen1 = ip_packet -> ip_hl << 2;			/* length of IP header */
	struct icmp *icmpy = (struct icmp *) (ptr + hlen1);			/* start of ICMP header */
	int icmplen = len - hlen1;
	if (icmplen < 8)
	{
		printf("icmplen (%d) < 8", icmplen);
	}
	if (icmpy->icmp_type == ICMP_ECHOREPLY)
	{
		if (icmpy->icmp_id != pid)
			return;
		/* not a response to our ECHO_REQUEST */
		if (icmplen < 16)
			printf("icmplen (%d) < 16", icmplen);
		char *data = (char *)icmpy->icmp_data;
		char tvsent[sizeof(struct timeval)];
		int i=0;
		for(i=0; i<sizeof(struct timeval); i++){
			tvsent[i] = *(data+i);
		}
		tv_sub(tvrecv, (struct timeval *)tvsent);
		float rtt = (float)(tvrecv->tv_sec) * 1000.0 + (float)(tvrecv->tv_usec) / 1000.0;
		char * msg = (char *)(icmpy->icmp_data) + sizeof(struct timeval);
		printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n", icmplen, host,icmpx->icmp_seq, ip_packet->ip_ttl, rtt);
		printf("Message returned: %s\n",msg);
	}
}
short in_cksum(u_short *addr, int len) {
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}

void tv_sub(struct timeval *recv, struct timeval *send)
{
	recv->tv_sec = recv->tv_sec - send->tv_sec;
	recv->tv_usec = recv->tv_usec - send->tv_usec;
	return;
}