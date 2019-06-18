#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>



int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	if(argc < 3)
	{
		printf("please usage %s <ip> <port>\n",argv[0]);
		exit(0);
	}
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("socket failed");
		exit(0);
	}
	printf("client socket ok .\n");
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	socklen_t addrlen = sizeof(serveraddr);
	
	int buf[1] = {0};
	while(1){
		buf[0]=(int)(rand()%11);
		if((sendto(sockfd,&buf,sizeof(buf),0,(struct sockaddr*)&serveraddr,addrlen))<0){
			perror("sendto failed");
			exit(0);
		}
	}
	return 0;
}
