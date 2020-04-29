all: main
main: main.o schedular.o
	gcc -std=c99 main.o schedular.o -o main
main.o: main.c common.h Makefile
	gcc -std=c99 -c main.c
schedular.o: schedular.c common.h Makefile
	gcc -std=c99 -c schedular.c

clean:
	rm -rf *.o main
run:
	sudo ./main