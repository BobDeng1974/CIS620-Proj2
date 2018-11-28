#include <stdio.h>
#include <proc.h>


int xidfoo, xidbar, xidA, xidB;
int x=0;

void A() {
	int i, j;
	for (i = 0; i < 3; i++) {
		printf("A %d\n", i);
		for (j = 0; j < 20000000; j++) {
			//busy waiting here, waiting for interrupt
		}
	}
}

void B() {
	int i, j;
	for (i = 0; i < 3; i++) {
		printf("B %d\n", i);
		for (j = 0; j < 20000000; j++) {
			//busy waiting here, waiting for interrupt
		}
	}
}

xthread_mbox_t * mbox; //global

void foo() {
	printf("foo begins\n");
	int msg; //, m;
	//printf("foo m before recv is %d\n", mbox->m);
	//xthread_print(mbox);
	//m = mbox->m;
	//printf("%d\n", m);
	xthread_recv(&mbox, &msg);
	printf("got: %d \n", msg);
	printf("foo end\n");
}

void bar() {
	int res;
	printf("bar begins\n");
	//printf("foo m before send is %d\n", mbox->m);
	//int m;
	//m = mbox->m;
	//printf("%d\n", m);
	res = xthread_send(&mbox, 777);
	if (res == -1) {
		printf("can't send message to full mailbox\n");
	}
	//printf("%d\n", *(mbox->m));
	printf("bar ends\n");
}

void bt() {
	int res;
	printf("bt begins\n");
	//printf("bt before broadcast\n");
	res = xthread_broadcast(&mbox, 777);
	if (res == -1) {
		printf("can't broadcast, mailbox is full\n");
	}
	printf("bt ends\n");
}

//have to use pointers here to use with malloc (which returns a pointer)
//***********can't malloc here because can't execute code outside of functions
//mbox = (xthread_mbox_t *)malloc(4 * 12 + sizeof(xthread_mbox_t));

xmain(int argc, char* argv[])
{
	//xidA = xthread_create(A, 0, NULL);
	//xidB = xthread_create(B, 0, NULL);

	//set m = 0, abs(s->m) is # of threads waiting to run
		//don't type cast malloc because it is automatic
//*************if use malloc, get seg fault because tid changes with m somehow
	mbox = calloc(1, sizeof(xthread_mbox_t));
	//mbox = malloc(500);
	xthread_init_mbox(mbox);
	//mbox->m = 0;
		//printf("%d\n", mbox->m); //still 0
		//xthread_print(mbox);
	xthread_create(bar, 0, NULL);
	xthread_create(foo, 0, NULL);
	xthread_create(bar, 0, NULL);
	xthread_create(foo, 0, NULL);
	xthread_create(bar, 0, NULL);
	//xthread_create(bar, 0, NULL);
//double broadcast works, it only doesn't work when preceded by recv

	//now to use the mbox to post and recv using foo and bar
	free(mbox);
}
