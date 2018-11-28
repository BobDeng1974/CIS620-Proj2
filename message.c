#include <proc.h>

int xthread_init_mbox(xthread_mbox_t * s) {
	s->head = NULL;
	//s->m = 0; //m is > 0 if it is a valid message
	//printf("init: m is %d\n", s->m);
	//s->msgptr = NULL;
	s->m = malloc(sizeof(int));
	//printf("%d\n", *(s->m));
	*(s->m) = 0;
	//printf("%d\n", *(s->m));
	s->tail = NULL;
	//cant access head->net because head hasn't been created yet
	//s->head->next = NULL; //this is causing seg fault
	//s->tail->next = NULL; //this is causing seg fault
	return 0;
}

void xthread_print(xthread_mbox_t * mptr) {
	printf("print func m is: %d\n", *(mptr->m));
}

void xthread_recv(xthread_mbox_t * s, int * msgptr) {
	//printf("recv m is initially: %d\n", *(s->m));
	int usec;
	usec = ualarm(0, 0); //disable timer
	//s->m = m;
	//printf("recv m is now: %d\n", *(s->m));
	//int currm = *(s->m);
	//printf("%d\n", currm);
	if (*(s->m) < 1) { //no valid message in the mailbox
		//printf("%d\n", *(s->m));
		struct node * new_node = NULL;
		//printf("%d\n", *(s->m));
//*********have to calloc or else tid and m CHANGES together?? why
		new_node = calloc(1, sizeof(node_t));
		//new_node->tid = malloc(sizeof(int));
		//*****printf("%d\n", *(s->m)); //setting new node tid also sets m??
		new_node->tid = currxid; //THIS IS THE PROBlEM
		//printf("%d\n", new_node->tid);
		//printf("after setting tid of new_node, m changes to %d\n", *(s->m)); //here u see m = tid
		new_node->msgptr = msgptr;
		//printf("%d\n", *(s->m));
		new_node->next = NULL;
		//printf("%d\n", new_node->tid); //new_node tid is 1
		//printf("%d\n", *(s->m));
		//*(s->m) = currm; //reset the message to currm
		//printf("%d\n", *(s->m));
		//printf("%d\n", new_node->tid); //new_node tid is 1
//now tid is 0, tid and m are connected together?????????????????
		if (s->head == NULL) { //no nodes
			s->head = new_node;
			//s->head->next = NULL;
			//printf("%d\n", s->head->tid);
			//s->head->tid = new_node->tid;
			//printf("%d\n", s->head->tid);
			s->tail = new_node;
			//s->tail->next = NULL;
		}
		else if (s->head == s->tail) { //one node
			s->head->next = new_node;
			//printf("%d\n", s->head->tid);
			s->tail = new_node;
			//s->tail->next = NULL;
		}
		else { //more than 1 node
			//just put at end of list
			s->tail->next = new_node;
			s->tail = new_node;
		}
		xtab[currxid].xstate = XMBOX;
		//printf("%d\n", *(s->m));
		//printf("%d\n", s->head->tid);
		resched();
		return;
	}
	else { //there is a message
		*msgptr = *(s->m); //get the message
		*(s->m) = 0; //reset message
		//printf("%d\n", *(s->m));
		ualarm(usec,0);
		return;
	}
}

int xthread_send(xthread_mbox_t * s, int msg) {
	//printf("send m is now: %d\n", *(s->m));
	//printf("%d\n", m);
	//s->m = m;
	//printf("%d\n", s->head->tid); //why is this 0?????? should be 1
	int usec, t_id;
	int * msgptr;
	usec = ualarm(0,0);
	if (*(s->m) < 1 && s->head == NULL) { //mailbox is empty, no one in queue
		*(s->m) = msg;
		//printf("send-> m is: %d, msg is: %d\n", *(s->m), msg);
		ualarm(usec, 0);
		return 0;
	}
	else if (s->head != NULL && *(s->m) < 1) { //means theres a thread waiting for messages
		t_id = s->head->tid;
		msgptr = s->head->msgptr;
		//printf("tid: %d, msgptr: %d\n", t_id, *(msgptr));
		//now dequeue the node, queue is FIFO
		//if queue only has 1 element
		if (s->head == s->tail) {
			free(s->head);
			s->head = NULL;
			s->tail = NULL;
		}
		else { //queue has more than one element
			free(s->head);
			s->head = s->head->next;
			//free up the memory?
		}
		*msgptr = msg;
		xtab[t_id].xstate = XREADY;
		return 0;
	}
	else {//mailbox is full
		ualarm(usec,0);
		return -1;
	}
}

int xthread_broadcast(xthread_mbox_t * s, int msg) {
	//printf("bt begin m %d\n", *(s->m));
	int usec, t_id;
	int * msgptr;
	usec = ualarm(0,0);
	//printf("bt 2nd m %d\n", *(s->m));
	if (*(s->m) > 0) { //mbox is full
		ualarm(usec, 0);
		return -1;
	}
	else { //mbox is not full
		//printf("bt inside else m %d\n", *(s->m));
		if (s->head == NULL) { //if no threads waiting
			//printf("bt else if\n");
			*(s->m) = msg;
			ualarm(usec, 0);
			return 0;
		}
		else { //there are threads waiting
			//printf("bt else else\n");
			node_t * ptr;
			ptr = s->head;

			while (ptr != NULL) {
				t_id = ptr->tid;
				msgptr = ptr->msgptr;
				*msgptr = msg;
				xtab[t_id].xstate = XREADY;
				free(ptr);
				ptr = ptr->next;
			}
			//need to set head and tail to NULL or else SEG FAULT
			s->head = NULL;
			s->tail = NULL;
		}
		//printf("end of broadcast\n");
		return 0;
	}
	//return 0;
}
