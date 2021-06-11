#include "headers.h"

void printNode(node *n)
{
    if(n == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%d\n", n->pcb.process.id);
        printNode(n->next);
    }
}

void printPriorityNode(prioritynode *n)
{
    if(n == NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("%d	%d \n", n->pcb.process.id,n->priorityofqueue);
        printPriorityNode(n->next);
    }
}

int main(int argc, char *argv[])
{
	printf("**************Testing Queue************\n");
  	queue *q;
    q = malloc(sizeof(queue));
    init(q);
    printNode(q->front);
    data d;
    PCB p;
    for(int i=1;i<8;i++)
    {
    	d.id=i;
  		p.process=d;
  		enqueue(q,p);
    }
    printNode(q->front);
    dequeue(q);dequeue(q);dequeue(q);
    printNode(q->front);
    dequeue(q);dequeue(q);dequeue(q);dequeue(q);
    printNode(q->front);
    
    printf("**************Testing Priority Queue************\n");
    priorityqueue *q1;
    q1 = malloc(sizeof(priorityqueue));
    initPriority(q1);
    printPriorityNode(q1->front);
    data d1;
    PCB p1;
    d1.id=1;
  	p1.process=d1;	
  	enqueuePriority(q1,p1,6);	
    d1.id=2;
  	p1.process=d1;	
  	enqueuePriority(q1,p1,3);	
    d1.id=3;
  	p1.process=d1;	
  	enqueuePriority(q1,p1,2);
  	d1.id=4;
  	p1.process=d1;	
  	enqueuePriority(q1,p1,4);
  	d1.id=5;
  	p1.process=d1;	
  	enqueuePriority(q1,p1,5);			
    printPriorityNode(q1->front);
    dequeuePriority(q1);dequeuePriority(q1);dequeuePriority(q1);
    printPriorityNode(q1->front);
    dequeuePriority(q1);dequeuePriority(q1);dequeuePriority(q1);dequeuePriority(q1);
    printPriorityNode(q1->front);
    
    
	return 0;
}
