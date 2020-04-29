#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"



int trans(char str[128]){
	if(strcmp(str,"FIFO")==0)
		return FIFO;
	if(strcmp(str,"RR")==0)
		return RR;
	if(strcmp(str,"SJF")==0)
		return SJF;
	if(strcmp(str,"PSJF")==0)
		return PSJF;
	else{
		fprintf(stderr, "%s\n", "policy_error");
		exit(0);
	}

}

int main(){
	char sche_policy[128];
	int policy;
	int n_pro;
	scanf("%s",sche_policy);
	scanf("%d",&n_pro);
	policy=trans(sche_policy);
	struct a_process *procs = (struct a_process*)malloc(n_pro*sizeof(struct a_process));
	for(int cnt=0;cnt<n_pro;cnt++){
		scanf("%s%d%d",procs[cnt].name,&procs[cnt].ready_t,&procs[cnt].exec_t);
	}
	schedule(procs,n_pro,policy);
	return 0;
}