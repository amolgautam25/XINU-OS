/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
 extern int flag_timer;
long ctr1000;
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[13]=process->count_sys[13]+1;
                 process->start_t[13] += ctr1000;
        }
		
	short	*nxtdev;


	if (isbadpid(pid)){
		
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[13] += ctr1000;
        }
	return(SYSERR);}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[13] += ctr1000;
        }
	return(OK);
}
