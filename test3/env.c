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

#define STO_NO 2


//数据转换后的环境信息
struct conver_env_info {
    int snum;		 //仓库编号
    float temperature;	 //温度	
    float humidity;		 //湿度
    float ill;		 //光照
    float bet;		 //电池电量
    float adc; 		 //电位器信息
    
    signed char x;			 //三轴信息
    signed char y;			 
    signed char z;			 
};

//int main(int argc, const char *argv[])
int cgiMain()
{
	
 	int shmid;
	key_t key;
	struct conver_env_info *shm_buf = NULL;
	struct conver_env_info env_msg;
	
	//get the same key
	key = ftok(".",'e');
	if(key == -1){
		perror("ftok failed.");
		exit(0);
	}
	printf("key=%d\n",key);

	//open share memory
	shmid = shmget(key,sizeof(struct conver_env_info),IPC_CREAT|IPC_EXCL|0666);
	if(shmid < 0){
		if(errno == EEXIST){
			shmid = shmget(key,0,0);
			printf("EXIST shmid = %d\n",shmid);
		}else{
			perror("share memory get failed.");
			exit(0);
		}
	}

	//attached memory
	shm_buf = (struct conver_env_info *)shmat(shmid,NULL,0);
	if(shm_buf == (struct conver_env_info *)-1){
		perror("attached memory failed.");
		exit(0);
	}

	
#if 0
	while(1){
		printf("envenvenvenvenv shm_buf=%d--y=%d--z=%d,temperature:%0.2lf,humidity:%0.2lf,ill:%0.2lf,bet:%0.2lf,adc:%0.2lf\n",\
				shm_buf->x,shm_buf->y,shm_buf->z,shm_buf->temperature,shm_buf->humidity,\
				shm_buf->ill,shm_buf->bet,shm_buf->adc);
		sleep(1);
	}
#endif

#if 1
	
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<head><meta http-equiv=\"refresh\" content=\"1\"><style><!--body{line-height:50%}--></style></head>");
	fprintf(cgiOut, "<HTML>\n");
	fprintf(cgiOut, "<BODY bgcolor=\"#666666\">\n");
	fprintf(cgiOut, "<h1><font color=\"#FF0000\">Storage #%d:</font></H2>\n ", STO_NO);
		fprintf(cgiOut, "<h2><font face=\"Broadway\"><font color=\"#FFFAF0\">Real-time information environment</font></font></H2>\n ");	
		fprintf(cgiOut, "<h4>Temperature:\t%0.2f</H4>\n ", shm_buf->temperature);
		fprintf(cgiOut, "<h4>Humidity:\t%0.2f</H4>\n ", shm_buf->humidity);
		fprintf(cgiOut, "<h4>Illumination:\t%0.2f</H4>\n ", shm_buf->ill);
		fprintf(cgiOut, "<h4>Battery:\t%0.2f</H4>\n ", shm_buf->bet);
		fprintf(cgiOut, "<h4>Adc:\t%0.2f</H4>\n ", shm_buf->adc);
		fprintf(cgiOut, "<h4>X:\t%d</H4>\n ", shm_buf->x);
		fprintf(cgiOut, "<h4>Y:\t%d</H4>\n ", shm_buf->y);
		fprintf(cgiOut, "<h4>Z:\t%d</H4>\n ", shm_buf->z);
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
