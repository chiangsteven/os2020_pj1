#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include "common.h"
#define FIFO 24
#define RR 21
#define SJF 23
#define PSJF 22

int last_t;
int running_proc_idx;
int n_t;
int fin_proc_cnt;

int next_proc(struct a_process *procs,int n_proc ,int policy);
int cmp(const void *a, const void *b) {
	return ((struct a_process *)a)->ready_t - ((struct a_process *)b)->ready_t;
}


void schedule(struct a_process *procs,int n_proc ,int policy){
	qsort(procs,n_proc,sizeof(struct a_process),cmp);
	for (int i = 0; i < n_proc; i++)
		procs[i].pid = -1;

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(getpid(), sizeof(mask), &mask);

	struct sched_param param;
	/* SCHED_OTHER should set priority to 0 */
	param.sched_priority = 0;
	sched_setscheduler(getpid(), SCHED_OTHER, &param);

	n_t=0;
	running_proc_idx=-1;//-1 means no proc
	fin_proc_cnt=0;

	while(1){
		if(running_proc_idx != -1 && procs[running_proc_idx].exec_t==0){
			waitpid(procs[running_proc_idx].pid,NULL,0);
			printf("%s %d\n", procs[running_proc_idx].name,procs[running_proc_idx].pid);
			running_proc_idx = -1;
			fin_proc_cnt++;
			//printf("fin_cnt=%d n_proc=%d\n", fin_proc_cnt,n_proc);
			if(fin_proc_cnt == n_proc){
				
				break;
			}
		}

		for(int i=0; i < n_proc ;i++){//find ready proc
			if(procs[i].ready_t == n_t){
				pid_t pid = fork();
				
				if(pid==0){
					unsigned long start_t,start_nt,end_t,end_nt;
					char buf_tok[256];
					struct timespec t;
					syscall(GETTIME,&t);
					start_t=t.tv_sec;
					start_nt=t.tv_nsec;
					for(int j=0;j<procs[i].exec_t;j++){
						unit_t();
					}
					syscall(GETTIME,&t);
					end_t=t.tv_sec;
					end_nt=t.tv_nsec;
					sprintf(buf_tok,"[Project1] %d %lu.%09lu %lu.%09lu\n",getpid(),start_t,start_nt,end_t,end_nt);
					syscall(PRINTK,buf_tok);
					exit(0);
				}

				CPU_ZERO(&mask);
				CPU_SET(1, &mask);
				sched_setaffinity(pid, sizeof(mask), &mask);
			
				procs[i].pid=pid;
				param.sched_priority = 0;
				sched_setscheduler(procs[i].pid, SCHED_IDLE, &param);
			}
		}

		int next_proc_idx = next_proc(procs,n_proc,policy);
		if(running_proc_idx!=next_proc_idx && next_proc_idx!= -1){//context switch
			param.sched_priority = 0;
			sched_setscheduler(procs[next_proc_idx].pid, SCHED_OTHER, &param);
			if(running_proc_idx!=-1){
				param.sched_priority = 0;
				sched_setscheduler(procs[running_proc_idx].pid, SCHED_IDLE, &param);
			}
			running_proc_idx = next_proc_idx;
			last_t = n_t;//to calculate proc running time 
		}

		unit_t();
		if(procs[running_proc_idx].pid!=-1)
			procs[running_proc_idx].exec_t--;

		n_t++;
	}
	return;
}

int next_proc(struct a_process *procs,int n_proc ,int policy){
	if(policy==FIFO){
		if(running_proc_idx!=-1)
			return running_proc_idx;
		else{
			int min_ready_t;
			int idx_min=-1;
			for(int i=0;i<n_proc;i++){
				if(procs[i].pid==-1 || procs[i].exec_t <=0)
					continue;
				if(idx_min==-1 || procs[i].ready_t < procs[idx_min].ready_t){
					idx_min = i;
					min_ready_t = procs[i].ready_t;
				}
			}
			return idx_min;
		}
	}
	else if(policy==SJF){
		if(running_proc_idx!=-1)
			return running_proc_idx;
		else{
			int idx_min=-1;
			for(int i=0;i<n_proc;i++){
				if(procs[i].pid==-1 || procs[i].exec_t <=0)
					continue;
				if(idx_min==-1 || procs[i].exec_t < procs[idx_min].exec_t){
					idx_min = i;
				}
			}
			return idx_min;
		}
	}
	else if(policy==PSJF){	
		int idx_min=-1;
		for(int i=0;i<n_proc;i++){
			if(procs[i].pid==-1 || procs[i].exec_t <=0)
				continue;
			if(idx_min==-1 || procs[i].exec_t < procs[idx_min].exec_t){
				idx_min = i;
			}
		}
		return idx_min;
	}
	else if(policy==RR){
		if(running_proc_idx==-1){
			for(int i=0;i<n_proc;i++){
				if(procs[i].pid!=-1 && procs[i].exec_t>0){
					return i;
				}
			}
		}
		else if((n_t-last_t)%500 == 0){
			int idx = (running_proc_idx+1)%n_proc;
			while(procs[idx].pid==-1 || procs[idx].exec_t <=0)
				idx = (idx+1)%n_proc;
			return idx;
		}
		else
			return running_proc_idx;
	}
}