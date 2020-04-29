#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_myprintk(char *msg){
	printk(msg);
}
