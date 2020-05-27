/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include<proc.h>
/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
 
 extern int flag_timer;
long ctr1000;


SYSCALL scount(int sem)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[10]=process->count_sys[10]+1;
                 process->start_t[10] += ctr1000;
        }
		
extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[10] += ctr1000;
        }
	return(SYSERR);}
	
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[10] += ctr1000;
        }
	return(semaph[sem].semcnt);
}
