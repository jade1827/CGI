#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/msg.h>


struct msgbuf{
	long mtype;
	int data;
};

int main(int argc, const char *argv[])
{
	key_t key;
	int msgid;
	int sockfd;
	struct msgbuf msg_s;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
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
	printf("server socket ok.\n");
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	socklen_t addrlen = sizeof(serveraddr);
	socklen_t clientlen = sizeof(clientaddr);
	if((bind(sockfd,(struct sockaddr *)&serveraddr,addrlen)) < 0){
		perror("bind failed");
		exit(0);
	}
	printf("server bind ok.\n");
	

	key = ftok(".",'a');
	if(key == -1){
		perror("failed to ftok,");
		exit(1);
	}
	printf("ftok ok.");

	msgid = msgget(key,IPC_CREAT|IPC_EXCL|0666);
	if(msgid < 0){
		if(errno == EEXIST){
			msgid = msgget(key,0);
		}else{
			perror("faile to msgget.");
			exit(1);
		}
	}

	msg_s.mtype = 100;
	int buf[1] = {0};
	while(1){
		recvfrom(sockfd,&buf,sizeof(buf),0,NULL,NULL);
		//printf("recv data is %d\n",buf[0]);
		msg_s.data = buf[0];
		msgsnd(msgid,&msg_s,sizeof(msg_s)-sizeof(msg_s.mtype),IPC_NOWAIT);
		sleep(1);
		}
	return 0;
}
