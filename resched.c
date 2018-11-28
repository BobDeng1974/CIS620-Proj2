/* resched.c  -  resched */
#include <stdio.h>
#include <proc.h>
#include <signal.h>
#include <unistd.h>

/*------------------------------------------------------------------------
 * resched  --  find a live thread to run
 *
 *------------------------------------------------------------------------
 */
void resched()
{
    register struct  xentry  *cptr;  /* pointer to old thread entry */
    register struct  xentry  *xptr;  /* pointer to new thread entry */
    int i,next;

    cptr = &xtab[currxid];

    next = currxid ;
    for(i=0; i<NPROC; i++) { 
        if( (++next) >= NPROC)
             next = 0;
        if(xtab[next].xstate == XREADY) {
            xtab[next].xstate = XRUN;
            xptr = &xtab[next];
            currxid = next;
			ualarm(100, 0);	//how many usec should a thread run
            ctxsw(cptr->xregs,xptr->xregs);
            return;
        }
    }
    printf("XT: no threads to run!\n");
    exit(0);
}

void handler() {
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &sigset, NULL);

	xtab[currxid].xstate = XREADY;
	resched();
}

