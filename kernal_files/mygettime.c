#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage void sys_mygettime(struct timespec *t){
	getnstimeofday(t);
}
