CC=arm-none-linux-gnueabi-gcc
all:
	$(CC) main.c -lpthread -o main
	$(CC) m0_led.c cgic.c -o m0_led.cgi
	$(CC) env.c cgic.c -o env2.cgi
.PHONY:clean
clean:
	rm *.o

