// few test cases might resemble the test cases provided in the assignment ( in case MOSS flags this code . Reference for the test cases : PA2 test_main.c)
// modifying few test cases from the above mentione file

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>
#define DEFAULT_LOCK_PRIO 20


void reader(char *msg, int sem)
{
	int ret;

	kprintf("  %s: to acquire lock\n", msg);
	wait(sem);
	kprintf("  %s: acquired lock\n", msg);
	kprintf("  %s: to release lock\n", msg);
	signal(sem);
}

void writer(char *msg, int sem)
{
	kprintf("  %s: to acquire lock\n", msg);
	wait(sem);
	kprintf("  %s: acquired lock, sleep 10s\n", msg);
	sleep(10);
	kprintf("  %s: to release lock\n", msg);
	signal(sem);
}

void sems_test()
{
	int sem;
	int rd1, rd2;
	int wr1;

	kprintf("\nsemaphore test\n");
	sem = screate(1);

	rd1 = create(reader, 2000, 25, "reader3", 2, "reader A", sem);
	rd2 = create(reader, 2000, 30, "reader3", 2, "reader B", sem);
	wr1 = create(writer, 2000, 20, "writer3", 2, "writer", sem);

	kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
	resume(wr1);
	sleep(1);

	kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
	resume(rd1);
	sleep(1);
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
	resume(rd2);
	sleep(1);
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	kprintf("-kill reader B, then sleep 1s\n");
	kill(rd2);
	sleep(1);
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	kprintf("-kill reader A, then sleep 1s\n");
	kill(rd1);
	sleep(1);
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	sleep(8);
	kprintf("Sems_test_end\n");
}
//------------------------------------------------------------------------------------------------



void reader3(char *msg, int lck)
{
	int ret;

	kprintf("  %s: to acquire lock\n", msg);
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock\n", msg);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}

void writer3(char *msg, int lck)
{
	kprintf("  %s: to acquire lock\n", msg);
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock, sleep 10s\n", msg);
	sleep(10);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}

void locks_test()
{
	int lck;
	int rd1, rd2;
	int wr1;

	kprintf("\nTest locks_test\n");
	lck = lcreate();
	//    assert (lck != SYSERR, "Test 3 failed");

	rd1 = create(reader3, 2000, 25, "reader3", 2, "reader A", lck);
	rd2 = create(reader3, 2000, 30, "reader3", 2, "reader B", lck);
	wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);

	kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
	resume(wr1);
	sleep(1);

	kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
	resume(rd1);
	sleep(1);
	//assert (getprio(wr1) == 25, "Test 3 failed");
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
	resume(rd2);
	sleep(1);
	//assert (getprio(wr1) == 30, "Test 3 failed");
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	kprintf("-kill reader B, then sleep 1s\n");
	kill(rd2);
	sleep(1);
	//assert (getprio(wr1) == 25, "Test 3 failed");
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	kprintf("-kill reader A, then sleep 1s\n");
	kill(rd1);
	sleep(1);
	//	assert(getprio(wr1) == 20, "Test 3 failed");
	kprintf("\n --> The updated priority of writer 1 ( pid = 46 ) is %d\n", getprio(wr1));

	sleep(8);
	kprintf("locks_test_end\n");
}

int main()
{
	locks_test();
	sems_test();

	shutdown();
}
