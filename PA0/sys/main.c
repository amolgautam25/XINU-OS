/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
/*int main()
{
/*	kprintf("\n\nHello World, Xinu lives\n\n");
 
 
   int parameter = 0xaabbccdd;
   //calling zfunction
   kprintf("0x%x",zfunction(parameter));
   kprintf("\n");
   // execution of zfunction is over
	
  //calling printsegaddres()
	printsegaddress();
	// execution of printdegaddress is over 
	
  // calling zfunction
	printtos();
  // execution of zfunction is over
  
  // calling printprocstks.c
  printprocstks(5);
  // execution of printprocstks finished 
  
  // execution of printsyscallsummary starts 
 
   //syscallsummary_start();
   //syscallsummary_stop();
   //printsyscallsummary();
	
	*/
	
	
	
//	return 0;
//}



int prX;
void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
prch(c)
char c;
{
	int i;
	sleep(5);	
}
int main()
{
	kprintf("Task 5 (printsyscallsummary)\n");
	syscallsummary_start();
	kprintf("starting system call");
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	
	sleep(10);
	syscallsummary_stop();
	printsyscallsummary();
	
	printf("this is the end of main process");
	return 0;
}

