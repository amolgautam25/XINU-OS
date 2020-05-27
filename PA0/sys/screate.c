/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

LOCAL int newsem();

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
 
extern int flag_timer;
long ctr1000;

SYSCALL screate(int count)
{
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->count_sys[15]=process->count_sys[15]+1;
                 process->start_t[15] += ctr1000;
        }
	STATWORD ps;    
	int	sem;

	disable(ps);
	if ( count<0 || (sem=newsem())==SYSERR ) {
		restore(ps);
		if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[15] += ctr1000;
        }
		return(SYSERR);
	}
	semaph[sem].semcnt = count;
	/* sqhead and sqtail were initialized at system startup */
	restore(ps);
	if(flag_timer==1){
                 struct pentry *process = &proctab[currpid];
                 process->stop_t[15] += ctr1000;
        }
	return(sem);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newsem()
{
	int	sem;
	int	i;

	for (i=0 ; i<NSEM ; i++) {
		sem=nextsem--;
		if (nextsem < 0)
			nextsem = NSEM-1;
		if (semaph[sem].sstate==SFREE) {
			semaph[sem].sstate = SUSED;
			return(sem);
		}
	}
	return(SYSERR);
}
