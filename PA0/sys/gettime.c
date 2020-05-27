/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include<proc.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;

SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[4]=process->count_sys[4]+1;
                 process->start_t[4] += ctr1000;
        }
		
		



if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[4] += ctr1000;
        }

    return OK;
}
