#include "headers.h"
///////////////// Global data members \\\\\\\\\\\\\\

queue *readyQueue;
priorityqueue *readyPriorityQueue;

PCB runningProcess;
int runningFlag=0;//0 not running process, 1 otherwise
FILE *f;
int count;
int quantum;
int algorithmnum;
key_t key_id;
int msgq_id;
struct msgbuff buff;
//////// get process from message buffer \\\\\\\\\\\\\\\\\

void receiveProcess()
{

    int rec_val;
    while(true)
    {
    	rec_val= msgrcv(msgq_id, &buff, sizeof(buff.d), 0, IPC_NOWAIT);
    	if(rec_val==-1)
    		break;
    	else
    	{
    		PCB receivedProcess;
    		receivedProcess.process=buff.d;
    		printf("RECEIVED ID : %d\n",receivedProcess.process.id);
    		receivedProcess.pid=-1;
    		receivedProcess.remain=buff.d.runningtime;
    		receivedProcess.firstStart=0;
    		receivedProcess.lastStart=0;
    		receivedProcess.finish=0;
    		receivedProcess.totalTime=0;
    		receivedProcess.TA=0;
    		receivedProcess.WTA=0;
    		receivedProcess.flag=1;
    		if(algorithmnum==1||algorithmnum==5)
    		{
    			enqueue(readyQueue,receivedProcess);
    		
    		}
    		else if(algorithmnum==2)
    		{
    			enqueuePriority(readyPriorityQueue,receivedProcess,receivedProcess.totalTime);
    
    		}
    		else if(algorithmnum==3)
    		{
    			enqueuePriority(readyPriorityQueue,receivedProcess,receivedProcess.process.priority);
    		
    		
    		}
    		else if(algorithmnum==4)
    		{
    			enqueuePriority(readyPriorityQueue,receivedProcess,receivedProcess.remain);
    		
    		
    		}
    	}
    }


}

///////////////// Round-Robin RR Algorithm \\\\\\\\\\\\\\\

void RR()
{
FILE *f3 = fopen("test.log", "a+");	
   	fprintf(f3,"ENTERED RR \n");
   	
   	fclose(f3);
	while(1)
	{int x = getClk();
		receiveProcess();
		//f3 = fopen("test.log", "a+");	
   		//fprintf(f3,"b3d receive \n");
   		//fclose(f3);
		if(runningFlag==0 && !isEmpty(readyQueue))
		{
			runningFlag=1;
			runningProcess=dequeue(readyQueue);
			//first time enter the scheduler need forking
			f3 = fopen("test.log", "a+");	
   			fprintf(f3,"ENTERED RR %d	%d 		%d \n",runningProcess.remain,runningProcess.process.runningtime,runningProcess.pid);
   	
   			fclose(f3);
   	//runningProcess.remain==runningProcess.process.runningtime)/
			if( runningProcess.pid==-1)
			{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"gowaaa RR %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
				
				f = fopen("scheduler.log", "a+");
				
   				fprintf(f,"At time %d process %d started arr %d total %d remanin %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				
   				fclose(f);
				int pid1=fork();
				if(pid1==-1)
					printf("EEEEEEEEEEE");
				else if(pid1==0)//child
				{
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					char string[40];
					char now[20];
					char remaining[20];
				// convert 123 to string [buf]
					sprintf(now,"%d", x);
					sprintf(remaining,"%d",runningProcess.remain);
					execl("./process.out", "process.out ",now,remaining,NULL);
					exit(0);
				
				}
				else//parent
				{
					
					runningProcess.pid=pid1;
				
				}
			
			
			
			}
			else
			{
				
				f = fopen("scheduler.log", "a+");	
   				fprintf(f,"At time %d process %d resumed arr %d total %d remanin %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				runningProcess.lastStart=x;
				kill(runningProcess.pid,SIGCONT); 
			
			}
		
		
		}
		else if (runningFlag==1)
		{
			if(runningProcess.remain>=quantum)
			{
				runningFlag=0;
				runningProcess.finish=getClk();
				runningProcess.remain-=(getClk()-runningProcess.lastStart);
				kill(runningProcess.pid,SIGCONT);
				if(runningProcess.remain==0)
				{
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remanin %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
					count++;
				
				}
				else
				{
					
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d stopped arr %d total %d remanin %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   					fclose(f);
					enqueue(readyQueue,runningProcess);
				
				}
				
				
				
				}
				
			}
		
		
		}
	
	
	
	}



///////////////// Shortest Remaining Time Next SRTN Algorithm \\\\\\\\\\\\

void SRTN()
{
	while(1)
	{int x = getClk();
		receiveProcess();
		if(runningFlag==0 && !isEmptyPriority(readyPriorityQueue))
		{
			runningFlag=1;
			runningProcess=dequeuePriority(readyPriorityQueue);
			//first time enter the scheduler need forking
			if(runningProcess.remain==runningProcess.process.runningtime)//&& runningProcess.pid==-1)
			{
				
				f = fopen("scheduler.log", "a+");
				
   				fprintf(f,"At time %d process %d started arr %d total %d remanin %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				
   				fclose(f);
				int pid1=fork();
				if(pid1==-1)
					printf("EEEEEEEEEEE");
				if(pid1==0)//child
				{
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					
					char now[20];
					char remaining[20];
				// convert 123 to string [buf]
					sprintf(now,"%d", x);
					sprintf(remaining,"%d",runningProcess.remain);
					execl("./process.out", "process.out ",now,remaining,NULL);
					exit(0);
				
				}
				else//parent
				{
					
					runningProcess.pid=pid1;
				
				}
			
			
			
			}
			else
			{
				
				f = fopen("scheduler.log", "a+");	
   				fprintf(f,"At time %d process %d resumed arr %d total %d remanin %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				runningProcess.lastStart=x;
				kill(runningProcess.pid,SIGCONT); 
			
			}
		
		
		}
		else if (runningFlag==1)
		{
			if(readyPriorityQueue->front!=NULL&&readyPriorityQueue->front->priorityofqueue < runningProcess.remain)
			{
				runningFlag=0;
				runningProcess.finish=getClk();
				runningProcess.remain-=(getClk()-runningProcess.lastStart);
				kill(runningProcess.pid,SIGCONT);
				if(runningProcess.remain==0)
				{
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remanin %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
					count++;
				
				}
				else
				{
					
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d stopped arr %d total %d remanin %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   					fclose(f);
					enqueue(readyQueue,runningProcess);
				
				}
				
				
				
				}
				
			}
		
		
		}
	
	
	
	}


int main(int argc, char *argv[])
{
	
   	//printf("hena 2");
	readyQueue = (queue*)malloc(sizeof(queue));//for RR,FCFS
	readyPriorityQueue= (priorityqueue*)malloc(sizeof(priorityqueue));//for SJF,HPF,SRTN
	init(readyQueue);
	initPriority(readyPriorityQueue);
	key_id = ftok("keyfile", 65);    
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    buff.mtype = 7;
    initClk();
	algorithmnum=atoi(argv[1]);
	quantum=atoi(argv[2]);
	count=atoi(argv[3]);
	count=count-1;//TO BE TESTED
	FILE *f2 = fopen("test.log", "a+");	
   	fprintf(f2,"ENTERED SCHEDULER \n");
   	fprintf(f2,"%d	%d	%d",algorithmnum,quantum,count);
   	fclose(f2);
	if(algorithmnum==1)//FCFS
	{
	
	
	
	}
	else if(algorithmnum==2)//SJF
	{
	
	
	}
	else if(algorithmnum==3)//HPF
	{
	
	
	}
	else if(algorithmnum==4)//SRTN
	{
		SRTN();
	
	}
	else if(algorithmnum==5)//RR
	{
		RR();
	}
	
    destroyClk(true);
}
