#include <proc.h>

void xthread_init_mbox(xthread_mbox_t * s) {
	s->head = NULL;
	s->m = 0; //m is > 0
	//s->msgptr = NULL;
	s->tail = NULL;
	//cant access head->net because head hasn't been createdd yet
	//s->head->next = NULL; //this is causing seg fault
	//s->tail->next = NULL; //this is causing seg fault
}


void xthread_recv(xthread_mbox_t * s, int * msgptr) {
	int usec;
	usec = ualarm(0, 0); //disable timer

	node_t * new_node = NULL;
	new_node = malloc(sizeof(node_t));
	new_node->tid = currxid;
	new_node->msgptr = msgptr;
	new_node->next = NULL;

	if (s -> m < 1) { //no valid message in the mailbox
		if (s->head == NULL) { //no nodes
			s->head = new_node;
			//s->head->next = NULL;
			s->tail = new_node;
			//s->tail->next = NULL;
		}
		else if (s->head == s->tail) { //one node
			s->head->next = new_node;
			s->tail = new_node;
			//s->tail->next = NULL;
		}
		else { //more than 1 node
			//just put at end of list
			s->tail->next = new_node;
			s->tail = new_node;
		}
		xtab[currxid].xstate = XMBOX;
		resched();
		return;
	}
	else { //there is a message
		*msgptr = s->m; //get the message
		s->m = 0; //reset message
		ualarm(usec,0);
	}
}

void xthread_send(xthread_mbox_t * s, int msg) {
	int usec, t_id;
	int * msgptr;
	usec = ualarm(0,0);
	if (s->m < 1) { //mailbox is empty
		*(s->m) = msg;
		ualarm(usec, 0);
	}
	else if (s->head != NULL) { //means theres a thread waiting for messages
		t_id = s->head->tid;
		msgptr = s->head->msgptr;
		//now dequeue the node, queue is FIFO
		//if queue only has 1 element
		if (s->head == s->tail) {
			s->head = NULL;
			s->tail = NULL;
		}
		else { //queue has more than one element
			s->head = s->head->next;
			//free up the memory?
		}
		xtab[t_id].xstate = XREADY;
	}
	else {//no thread waiting to receive

		ualarm(usec,0);
	}
}

void xthread_broadcast(xthread_mbox_t * s, int msg) {
	printf("hi");
}
