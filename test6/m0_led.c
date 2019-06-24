#include "cgic.h"

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<sys/sem.h>
#include<sys/msg.h>

#define STO_NO 2
#define N 8

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

struct msgbuf{
	long mytype;
	uint8_t cmd;
};

//仓库编号
#define STORE1  0x40
#define STORE2  0x80
#define STORE3  0xc0

//设备编号
#define FAN  0x00
#define BEEP 0x10
#define LED  0x20

enum MO_CMD{
    FAN_OFF = STORE2|FAN|0x00,
    FAN_1 = STORE2|FAN|0x01,
    FAN_2 = STORE2|FAN|0x02,
    FAN_3 = STORE2|FAN|0x03,

    BEEP_OFF = STORE2|BEEP|0x00,
    BEEP_ON = STORE2|BEEP|0x01,
    BEEP_ALRRM_OFF = STORE2|BEEP|0x02,
    BEEP_ALRRM_ON = STORE2|BEEP|0x03,

    LED_OFF = STORE2|LED|0x00,
    LED_ON = STORE2|LED|0x01,
};


//int main(int argc, const char *argv[])
int cgiMain()
{	
 	int msgid;
	key_t key;
	struct msgbuf msg_scmd;
	char buf[N];
	
	//get a key
	key = ftok(".",'e');
	if(key == -1){
		perror("ftok failed.");
		exit(0);
	}

	//get msgid
	msgid = msgget(key,IPC_CREAT|IPC_EXCL|0666);
	if(msgid < 0){
		if(errno == EEXIST){
			msgid = msgget(key,0);
		}else{
			perror("failed to msgget.");
			exit(0);
		}
	}

#if 1
	cgiFormString("led",buf,N);	
	bzero(&msg_scmd,sizeof(msg_scmd));
	
	if(buf[0] == '1'){
		msg_scmd.mytype = 100;
		msg_scmd.cmd = LED_ON;
		msgsnd(msgid,&msg_scmd,sizeof(msg_scmd)-sizeof(msg_scmd.mytype),IPC_NOWAIT);
	}else{		
		msg_scmd.mytype = 100;
		msg_scmd.cmd = LED_OFF;
		msgsnd(msgid,&msg_scmd,sizeof(msg_scmd)-sizeof(msg_scmd.mytype),IPC_NOWAIT);
	}
	
	cgiHeaderContentType("test/html\n\n");
	fprintf(cgiOut,"<HTML><HEAD>\n");
	fprintf(cgiOut,"<TITLE>MY CGI</TITLE></HEAD>\n");
	fprintf(cgiOut,"<BODY>\n");
	fprintf(cgiOut,"<H2>send led success</H2>\n");
	
	fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"1;url=m0_1.html\">",2);
	fprintf(cgiOut,"</BODY>\n");
	fprintf(cgiOut,"</HTML>\n");

#endif
	return 0;
}
