//printprocstks.c

#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long *esp;

void printprocstks(int priority)

{
	kprintf("void printprocstks(int priority)");
	kprintf("\n");
	
	int counter;
	struct pentry *process;
	
	for ( counter =0 ; counter <NPROC ; counter++ )
	{
		process=&proctab[counter];
		
		if((process -> pstate != PRFREE) && (process -> pprio > priority))
		{
			kprintf("Process [%s]\n", process -> pname);
			kprintf("\tpid: %d\n",counter);
			kprintf("\tpriority: %d\n",process -> pprio);	
			kprintf("\tbase: 0x%08x\n",process -> pbase);
			kprintf("\tlimit: 0x%08x\n",process -> plimit);
			kprintf("\tlen: %d\n",process -> pstklen);
			
			if(process->pstate == PRCURR)
				{
					asm("movl %esp, esp");
					kprintf("\t0x%08x\n",esp);
				}
				else 
				{
					esp = (unsigned long *)process->pesp;
					kprintf("\t0x%08x\n",esp);
				}
			
		}
		
	}
	
	return ;
}
