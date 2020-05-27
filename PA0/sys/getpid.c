/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
 
extern int flag_timer;
long ctr1000;
SYSCALL getpid()
{
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[2]=process->count_sys[2]+1;
                 process->start_t[2] += ctr1000;
        }
		
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[2] += ctr1000;
        }
		
	return(currpid);
}
