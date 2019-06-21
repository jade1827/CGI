#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<pthread.h>

#define DEV_UART 			"/dev/ttyUSB0"


typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;
uint8_t cmd;

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



int dev_uart_fd;
pthread_t thread_send_cmd;
int a = 7;

//fan control
void fan_control(int level){
	switch(level){
	case 1:
		cmd = FAN_1;
		break;
	case 2:
		cmd = FAN_2;
		break;
	case 3:
		cmd = FAN_3;
		break;
	case 0:
		cmd = FAN_OFF;
		break;
	default:
		cmd = FAN_OFF;
	}
	write(dev_uart_fd, &cmd, sizeof(cmd));
}

//led control
void led_control(int on){
	if(on == 1){
		cmd = LED_ON;
	}else{
		cmd = LED_OFF;
	}
	write(dev_uart_fd, &cmd, sizeof(cmd));
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

void *send_cmd(void * arg){
	//open char_driver_file
    if((dev_uart_fd = open (DEV_UART, O_RDWR)) < 0)
    {
        perror ("open uart err");
        return;
    }
	//serial init
	serial_init (dev_uart_fd);
	
	while(1){
		led_control(1);
		sleep(3);
		led_control(0);
		sleep(3);
	}
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{


	//pthread -- send cmd control sensor
	if(pthread_create(&thread_send_cmd,NULL,send_cmd,&a) != 0){
		perror("pthread send cmd failed.");
		exit(0);
	}
	printf("pthread -------send_cmd\n");
	
	pthread_join(thread_send_cmd,NULL);

	return 0;
}

