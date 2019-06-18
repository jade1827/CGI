#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <unistd.h>
#include "cgic.h"

#define N 32

#define STO_NO 2



struct msgbuf{
	long mtype;
	int data;
};

int cgiMain()

//int main(int argc, const char *argv[])
{
	key_t key;
	int msgid;
	struct msgbuf msg_r;
	
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
	
	msg_r.mtype = 100;
#if 0
	while(1){
		msgrcv(msgid,&msg_r,sizeof(msg_r)-sizeof(msg_r.mtype),0,0);
		printf("------recvdata = %d\n",msg_r.data);
	}
#else 

	msgrcv(msgid,&msg_r,sizeof(msg_r)-sizeof(msg_r.mtype),0,0);
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<head><meta http-equiv=\"refresh\" content=\"1\"><style><!--body{line-height:50%}--></style></head>");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<BODY bgcolor=\"#666666\">\n");
	fprintf(cgiOut, "<h1><font color=\"#FF0000\">Storage #%d:</font></H2>\n ", STO_NO);
	fprintf(cgiOut, "<h4>RecvData:\t%d</H4>\n ", msg_r.data);
	fprintf(cgiOut, "</BODY></HTML>\n");	
#endif
	/*
	if(msgctl(msgid,IPC_RMID,NULL) < 0){
		perror("faile to msgctl");
		exit(0);
	}
	*/
//	cgiHeaderContentType("text/html");
//	fprintf(cgiOut, "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><style type = \"text/css\"><!--body{line-height:50%}--></style></head>");
//	fprintf(cgiOut, "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><style><!--body{line-height:50%}--></style></head>");
//	fprintf(cgiOut, "<head><meta http-equiv=\"refresh\" content=\"1\"><style><!--body{line-height:50%}--></style></head>");
//	fprintf(cgiOut, "<HTML>\n");
//	fprintf(cgiOut, "<BODY bgcolor=\"#666666\">\n");
//	fprintf(cgiOut, "<h1><font color=\"#FF0000\">Storage #%d:</font></H2>\n ", STO_NO);

#if 0
	if (shm_buf->storage_status == 1)
	{
		fprintf(cgiOut, "<h2><font face=\"Broadway\"><font color=\"#FFFAF0\">Real-time information environment</font></font></H2>\n ");	
		fprintf(cgiOut, "<h4>Temperature:\t%0.2f</H4>\n ", shm_buf->temperature);
		fprintf(cgiOut, "<h4>Humidity:\t%0.2f</H4>\n ", shm_buf->humidity);
		fprintf(cgiOut, "<h4>Illumination:\t%0.2f</H4>\n ", shm_buf->ill);
		fprintf(cgiOut, "<h4>Battery:\t%0.2f</H4>\n ", shm_buf->bet);
		fprintf(cgiOut, "<h4>Adc:\t%0.2f</H4>\n ", shm_buf->adc);
		fprintf(cgiOut, "<h4>X:\t%d</H4>\n ", shm_buf->x);
		fprintf(cgiOut, "<h4>Y:\t%d</H4>\n ", shm_buf->y);
		fprintf(cgiOut, "<h4>Z:\t%d</H4>\n ", shm_buf->z);
		//fprintf(cgiOut, "<h4>Buzzer:\t%s</H4>\n ", status[shm_buf->buzzer_status]);
		//fprintf(cgiOut, "<h4>Led:\t%s</H4>\n ", status[shm_buf->led_status]);
		//fprintf(cgiOut, "<h4>Fan:\t%s</H4>\n ", fan_status[shm_buf->fan_status]);
		//fprintf(cgiOut, "<h4>Temperature\nMAX: %0.0f MIN: %0.0f</H4>\n ", shm_buf->temperatureMAX, shm_buf->temperatureMIN);
		//fprintf(cgiOut, "<h4>Humidity\nMAX: %0.0f MIN: %0.0f</H4>\n ", shm_buf->humidityMAX, shm_buf->humidityMIN);
		//fprintf(cgiOut, "<h4>Illumination\nMAX: %0.0f MIN: %0.0f</H4>\n ", shm_buf->illuminationMAX, shm_buf->illuminationMIN);

	}
	else
	{
		fprintf(cgiOut, "<h2><font face=\"Broadway\"><font color=\"#FFFAF0\">Close!</font></font></H2>\n ");	
	}
#endif

//		fprintf(cgiOut, "<h4>TestData:\t%d</H4>\n ", recvdata);
	//	fprintf(cgiOut, "<h4>Humidity:\t%0.2f</H4>\n ", j);
	//	fprintf(cgiOut, "<h4>Illumination:\t%0.2f</H4>\n ", m);
//	fprintf(cgiOut, "<h3>:</H3>\n ");	
//	fprintf(cgiOut, "</BODY></HTML>\n");	

	return 0;
}
