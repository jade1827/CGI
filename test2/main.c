
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/sem.h>
#include<sys/shm.h>

#define N 32
#define READ 0
#define WRITE 1
#define STO_NO 2


union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
	struct seminfo *__buf;
};


//function init sem
void init_sem(int semid, int s[], int n){
	int i;
	union semun myun;
	for(i=0; i<n; i++){
		myun.val = s[i];
		semctl(semid,i,SETVAL,myun);
	}
}

//function pv action
void mypv(int semid, int num, int op){
	struct sembuf buf;

	buf.sem_num = num;
	buf.sem_op = op;
	buf.sem_flg = 0;//IPC_NOWAIT
	semop(semid,&buf,1);
}

int main(int argc, const char *argv[])
{
	 key_t key;
	 int semid;
	 int shmid;
	 int s[]={0,1};
	 int *shmaddr;
	int sockfd;

	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	//check input argument
	if(argc < 3)
	{
		printf("please usage %s <ip> <port>\n",argv[0]);
		exit(0);
	}
	//socket to get scokfd
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("socket failed");
		exit(0);
	}
	printf("server socket ok.\n");

	//fill scokaddr struct
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	socklen_t addrlen = sizeof(serveraddr);
	socklen_t clientlen = sizeof(clientaddr);

	//bind
	if((bind(sockfd,(struct sockaddr *)&serveraddr,addrlen)) < 0){
		perror("bind failed");
		exit(0);
	}
	printf("server bind ok.\n");

	//get key for IPC connect
	key = ftok(".",'a');
	if(key == -1){
		perror("failed to ftok,");
		exit(1);
	}
	printf("ftok ok.");

	//create share memory
	shmid = shmget(key,N,IPC_CREAT|IPC_EXCL|0666);
	if(shmid < 0){
		if(errno == EEXIST){
			shmid = shmget(key,0,0);
		}else{
			perror("share memory get failed.");
			exit(0);
		}
	}

	//create sem
	semid = semget(key,2,IPC_CREAT|IPC_EXCL|0666);
	if(semid < 0){
		if(errno == EEXIST){
			semid = semget(key,0,0);
		}else{
			perror("sem get failed.");
			goto _error1;
		}
	}
	init_sem(semid,s,2);

	//attached memory
	shmaddr = (int *)shmat(shmid,NULL,0);
	if(shmaddr == (int *)-1){
		perror("attached memory failed.");
		goto _error2;
	}

	int buf[1] = {0};
	while(1){
		recvfrom(sockfd,&buf,sizeof(buf),0,NULL,NULL);
		mypv(semid,WRITE,-1);
		*shmaddr = buf[0];
		//printf("in shame is %d\n",*shmaddr);
		//fgets(shmaddr,N,&buf[0]);
		mypv(semid,READ,1);
	}


_error2:
	semctl(semid,0,IPC_RMID);

_error1:
	shmctl(shmid,IPC_RMID,NULL);

	return 0;
}

