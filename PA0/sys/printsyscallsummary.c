//printsyscallsummary.c

#include<stdio.h>
#include<kernel.h>
#include<proc.h>
#include<lab0.h>


extern int flag_timer;


void syscallsummary_start()
{
	flag_timer=1;
	int rows,coloumn;
	
	for (rows = 0 ; rows < NPROC ; rows ++)
	{
		struct pentry *process = &proctab[rows];
		for (coloumn =0 ; coloumn<27; coloumn++)
		{
			process->start_t[coloumn]=0;
			process->stop_t[coloumn]=0;
			process->count_sys[coloumn]=0;
		}
	}
	
	kprintf("\nsyscall summary start finished \n");
	return;
}

void syscallsummary_stop()
{
	flag_timer=0;
	// no need to make any changes in the process table at this point , we will read the information stored in the process table in printsyscallsummary
	kprintf("\nsyscall stop finished \n");
	return;
}

char proc_name[27][NPROC]={"freemem","chprio","getpid","getprio","gettime","kill","receive","recvclr","recvtim","resume","scount","sdelete","send","setdev","setnok","screate","signal","signaln","sleep","sleep10","sleep100","sleep1000","sreset","stacktrace","suspend","unsleep","wait"};

void printsyscallsummary(){
	printf("\nvoid printsyscallsummary()\n");
	int i = 0, j = 0;
	for(j=1; j<NPROC; j++){
		struct pentry *process = &proctab[j];
		if(strcmp(process->pname,"") != 0)
			printf("Process [pid:%d]\n",j);
		for(i=0; i<27; i++){
			if(process->count_sys[i]>0){
				 long exec_time = 0;
				 if(process->start_t[i] <= process->stop_t[i]){
					exec_time = (process->stop_t[i] - process->start_t[i])/process->count_sys[i];}
				 printf("\tSyscall: %s, count: %d, average execution time: %d (ms)\n",proc_name[i],process->count_sys[i],exec_time);
			}
       		 }		
	
	}
}