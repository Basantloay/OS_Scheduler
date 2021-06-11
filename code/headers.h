#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

////////////////////// Process \\\\\\\\\\\\\\\\\\\\\\\\

typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
}data;

typedef struct processControlBlock
{
	data process;
	int pid;
	int wait;
	int remain;
	int firstStart;
	int lastStart;
	int finish;
	//int totalTime;
	int TA;
	float WTA;
	int flag;//if -1 infer data is empty or contain garbage
			 // 1 otherwise
	
}PCB;

////////////////// Data structures \\\\\\\\\\\\\\\\\\\\

////////////////// Node \\\\\\\\\\\\\\\\

typedef struct Node
{
    PCB pcb ;
    struct Node *next;
}node;

node* createNode( PCB pcb1) 
{
	node* n = malloc(sizeof(node));
	n->pcb=pcb1;
	n->next=NULL;
	return n;
}
	
typedef struct PriorityNode
{
    PCB pcb ;
    struct PriorityNode *next;
    int priorityofqueue;
}prioritynode;

prioritynode* createPriorityNode( PCB pcb1,int priorityofqueue1) 
{
	prioritynode* n = malloc(sizeof(prioritynode));
	n->pcb=pcb1;
	n->next=NULL;
	n->priorityofqueue=priorityofqueue1;
	return n;
}	  
////////////////// Queue \\\\\\\\\\\\\\\\

typedef struct Queue
{
    node *front;
    node *rear;
    long count;
} queue;

void init(queue *q)
{
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

bool isEmpty(queue *q)
{
	if(q->front==NULL && q->rear==NULL)
		return true;
	else
		return false;

}

void enqueue(queue *q, PCB p1)
{
	p1.flag=1;
	node* n=createNode(p1);
	//Empty queue
	if (q->front==NULL && q->rear==NULL)
	{
		q->front = n;
    	q->rear = n;
    	q->count+=1;
	}
	else
	{
		q->rear->next = n;
        q->rear = n;
        q->count+=1;
		
	}
	
}


PCB dequeue(queue *q)
{
	PCB p;
	p.flag=-1;
	if (q->front==NULL && q->rear==NULL)
	{
		return p;
	}
	else
	{
		node *n=q->front;
		p=n->pcb;
		q->front = q->front->next; 
		if (q->front == NULL) 
		    q->rear = NULL; 
		free(n);
	  	return p;
	
	}
}

////////////////// Priority Queue \\\\\\\\\\\\\\\\

typedef struct PriorityQueue
{
    prioritynode *front;
    prioritynode *rear;
    long count;
} priorityqueue;

void initPriority(priorityqueue *q)
{
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

bool isEmptyPriority(priorityqueue *q)
{
	if(q->front==NULL && q->rear==NULL)
		return true;
	else
		return false;

}

void enqueuePriority(priorityqueue *q, PCB p,int priorityofqueue)
{
	p.flag=1;
	prioritynode* pn=createPriorityNode(p,priorityofqueue);
	//Empty Priority queue
	if (q->front==NULL && q->rear==NULL)
	{
		//printf("eeeeeeee\n");
		q->front = pn;
    	q->rear = pn;
    	q->count+=1;
	}
	else
	{	
		if(pn->priorityofqueue < q->front->priorityofqueue)
		{
		//printf("first\n");
			
			pn->next=q->front;
	 		q->front=pn;
		
		}
		else if(pn->priorityofqueue >= q->rear->priorityofqueue )
	 	{
	 	//printf("second\n");
	 		q->rear->next=pn;
	 		q->rear=pn;
	 		
	 	}
	 	
		else
		{
		//printf("AAAA\n");
		prioritynode* temp=q->front;
		prioritynode* tempnext=q->front->next;
		while(temp!=NULL && tempnext!=NULL && temp->next->priorityofqueue <= pn->priorityofqueue)
		{
			temp=temp->next;
			tempnext=temp->next;
		}
		pn->next=temp->next;
	 	temp->next=pn;
	 	}
	}
	
}


PCB dequeuePriority(priorityqueue *q)
{
	PCB p;
	p.flag=-1;
	if (q->front==NULL && q->rear==NULL)
	{
		return p;
	}
	else
	{
		prioritynode *n=q->front;
		p=n->pcb;
		q->front = q->front->next; 
		if (q->front == NULL) 
		    q->rear = NULL; 
		free(n);
	  	return p;
	
	}
}



