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
#include<pthread.h>
#include<sys/sem.h>

#define STO_NO 2

#define DEV_UART 			"/dev/ttyUSB0"


typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
	struct seminfo *__buf;
};

struct env_info
{
	uint8_t head[3];	 //标识位st:
	uint8_t type;		 //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t temp[2];	 //温度	
	uint8_t hum[2];		 //湿度
	uint8_t x;			 //三轴信息
    uint8_t y;
    uint8_t z;
	uint32_t ill;		 //光照
	uint32_t bet;		 //电池电量
	uint32_t adc; 		 //电位器信息
};

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


//function init sem
void init_sem(int semid, int s, int n){
	union semun myun;
	myun.val = n;
	semctl(semid,s,SETVAL,myun);
}


//function pv action 
void mypv(int semid, int num, int op){
	struct sembuf buf;

	buf.sem_num = num;
	buf.sem_op = op;
	buf.sem_flg = 0;
	semop(semid,&buf,1);
}


static float dota_atof (char unitl)
{
    if (unitl > 100)
    {
        return unitl / 1000.0;
    }
    else if (unitl > 10)
    {
        return unitl / 100.0;
    }
    else
    {
        return unitl / 10.0;
    }
}

static int dota_atoi (const char *cDecade)
{
    int result = 0;
    if (' ' != cDecade[0])
    {
        result = (cDecade[0] - 48) * 10;
    }
    result += cDecade[1] - 48;
    return result;
}

static float dota_adc (unsigned int ratio)
{
    return ((ratio * 3.3) / 1024);
}


void serial_init(int fd)
{
    struct termios options;
    tcgetattr(fd, &options);
    options.c_cflag |= ( CLOCAL | CREAD );
    options.c_cflag &= ~CSIZE;
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB;
    options.c_iflag |= IGNPAR;
    options.c_iflag &= ~(ICRNL | IXON);
    options.c_oflag = 0;
    options.c_lflag = 0;

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    tcsetattr(fd,TCSANOW,&options);
}


	struct conver_env_info env_msg;
	struct env_info envinfo;
    int ret;
	int dev_uart_fd;
	key_t key;
	int shmid,semid;
	struct conver_env_info *shmaddr = NULL;
	pthread_t thread_recv,thread_sham;
	int a = 7;

void *recv_data(void * arg){
	//open char_driver_file
    if((dev_uart_fd = open (DEV_UART, O_RDWR)) < 0)
    {
        perror ("open uart err");
        return;
    }
	//serial init
	serial_init (dev_uart_fd);

	while(1){
	//	read data from file
		ret = read(dev_uart_fd, &envinfo, sizeof(struct env_info));
        if(ret != sizeof(envinfo)){
            continue;
        }
	//cover data and fill struct
        env_msg.x = envinfo.x;
        env_msg.y = envinfo.y;
        env_msg.z = envinfo.z;
        env_msg.temperature = envinfo.temp[0] + dota_atof(envinfo.temp[1]);
        env_msg.humidity = envinfo.hum[0] + dota_atof(envinfo.hum[1]);
        env_msg.ill = envinfo.ill;
        env_msg.bet = dota_adc(envinfo.bet);
        env_msg.adc = dota_adc(envinfo.adc);
    }
	pthread_exit(NULL);
}

void *write_sham(void * arg){
	
		while(1){
		//write to share memory
	mypv(semid,0,-1);
		shmaddr->x = env_msg.x;
		shmaddr->y = env_msg.y;
		shmaddr->z = env_msg.z;
		shmaddr->temperature = env_msg.temperature;
		shmaddr->humidity = env_msg.humidity;
		shmaddr->ill = env_msg.ill;
		shmaddr->bet = env_msg.bet;
		shmaddr->adc = env_msg.adc;
/*	
		printf("&&&&&&&&&env_msg.x=%d--y=%d--z=%d,temperature:%0.2lf,humidity:%0.2lf,ill:%0.2lf,bet:%0.2lf,adc:%0.2lf\n",\
				shmaddr->x,shmaddr->y,shmaddr->z,shmaddr->temperature,shmaddr->humidity,\
				shmaddr->ill,shmaddr->bet,shmaddr->adc);
		sleep(1);
*/		
	mypv(semid,0,1);
	}
	pthread_exit(NULL);

}

int main(int argc, const char *argv[])
{

	//get key
	key = ftok(".",'e');
	if(key == -1){
		perror("ftok failed.");
		exit(0);
	}

	//create share memory
	shmid = shmget(key,sizeof(struct conver_env_info),IPC_CREAT|IPC_EXCL|0666);
	if(shmid < 0){
		if(errno == EEXIST){
			shmid = shmget(key,0,0);
		}else{
			perror("share memory failed.");
			exit(0);
		}
	}

	//get semid
	semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
	if(semid < 0){
		if(errno == EEXIST){
			semid = semget(key,0,0);
		}else{
			perror("sem get failed.");
			goto _error1;
		}
	}
	init_sem(semid,0,1);

	//attached memory
	shmaddr = (struct conver_env_info *)shmat(shmid,NULL,0);
	if(shmaddr == (struct conver_env_info *)-1){
		perror("attached memory failed.");
		goto _error2;
	}

	//pthread --- receive data form serial
	if(pthread_create(&thread_recv,NULL,recv_data,&a) != 0){
		perror("pthread received data failed.");
		exit(0);
	}
	printf("pthread -------A\n");

	//pthread --- wirte to share memeory
	if(pthread_create(&thread_sham,NULL,write_sham,&a) != 0){
		perror("ptread wirte share memeory failed.");
		exit(0);
	}
	printf("pthread -------B\n");

	pthread_join(thread_recv,NULL);
	pthread_join(thread_sham,NULL);

_error2:
	semctl(semid,0,IPC_RMID);

_error1:
	shmctl(shmid,IPC_RMID,NULL);

	return 0;
}

