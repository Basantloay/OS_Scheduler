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
	int wait;
	int remain;
	int firstStart;
	int lastStart;
	int finish;
	int totalTime;
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
	
	  
////////////////// Queue \\\\\\\\\\\\\\\\

typedef struct Queue
{
    node *front;
    node *rear;
    long count;
} queue;
/*
queue* createQueue() 
{
	queue* q = malloc(sizeof(queue));
	q->front=NULL;
	q->rear=NULL;
	q->count=0;
	return q;
}
*/
void init(queue *q)
{
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

void enqueue(queue *q, PCB p)
{
	node* n=createNode(p);
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


