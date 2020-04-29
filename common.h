#ifndef _COMMOM_H_
#define _COMMOM_H_
#include <sys/types.h>
#define FIFO 24
#define RR 21
#define SJF 23
#define PSJF 22
#define PRINTK 333
#define GETTIME 334

#define unit_t() { volatile unsigned long iii; for(iii=0;iii<1000000UL;iii++); } 

struct a_process{
	char name[32];
	int ready_t;
	int exec_t;
	pid_t pid;
};
#endif