/*    proc.h    */
#include <stdio.h>
#include <stdlib.h>

typedef int WORD;

#define PNREGS 5

#define NPROC 10

#define SP     0

/* state */
#define XFREE    0
#define XREADY   1
#define XRUN     2
#define XMBOX    3

struct xentry  {
       int  xid;
       WORD xregs[PNREGS];    /* save SP */
       WORD xbase;
       WORD xlimit;
       int  xstate;
};

#define STKSIZE 8192

extern struct xentry xtab[];

extern int currxid;

typedef struct node {
	int tid; //tried to make this to a pointer type but doesnt work
  //have to use pointer here
	struct node * next;
  int * msgptr;
}node_t;

typedef struct xthread_mbox {
  int * m;
  struct node * head;
  struct node * tail;
  //int * msgptr;
}xthread_mbox_t;

/*
struct sema_s {
	int m;
	struct Node * head;
	struct Node * tail;
} sema_t;
*/
