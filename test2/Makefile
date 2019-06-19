all:
	gcc mydata.c -o mydata
	arm-none-linux-gnueabi-gcc env.c cgic.c -o env2.cgi
	arm-none-linux-gnueabi-gcc main.c -o main
.PHONY:clean
clean:
	rm *.o

