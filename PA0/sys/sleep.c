/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
 
extern int flag_timer;
long ctr1000;
 
SYSCALL	sleep(int n)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[18]=process->count_sys[18]+1;
                 process->start_t[18] += ctr1000;
        }
	STATWORD ps;    
	if (n<0 || clkruns==0)
	{
if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[18] += ctr1000;
        }		
	
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[18] += ctr1000;
        }
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[18] += ctr1000;
        }
	return(OK);
}
