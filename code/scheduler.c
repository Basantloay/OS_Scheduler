#include "headers.h"
///////////////// Global data members \\\\\\\\\\\\\\

queue *readyQueue = malloc(sizeof(queue));//for RR,FCFS
init(readyQueue);
queue *readyPriorityQueue = malloc(sizeof(priorityqueue));//for SJF,HPF,SRTN
initPeiority(readyPriorityQueue);
PCB runningProcess=NULL;
int runningFlag=0;//0 not running process, 1 otherwise
FILE *f;
int count;
int quantum;
//To Be Inplemented later
void receiveProcess()
{
	



}

///////////////// Round-Robin RR Algorithm \\\\\\\\\\\\\\\

void RR()
{
	while(1)
	{int x = getClk();
		receiveProcess();
		if(runningFlag==0 && !isEmpty(readyQueue))
		{
			runningFlag=1;
			runningProcess=dequeue(readyQueue);
			//first time enter the scheduler need forking
			if(runningProcess.remain==runningProcess.process.runningtime)//&& runningProcess.pid==-1)
			{
				
				f = fopen("scheduler.log", "w+");
				
   				fprintf(f,"At time %d process %d started arr %d total %d remanin %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				
   				fclose(f);
				int pid1=fork();
				if(pid1==-1)
					printf("EEEEEEEEEEE");
				if(pid1==0)//child
				{
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					char string[40];
					char now[20];
					char remaining[20];
				// convert 123 to string [buf]
					itoa(x, now, 20);
					itoa(runningProcess.remain,remaining,20);
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
				
				f = fopen("scheduler.log", "w+");	
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
					f = fopen("scheduler.log", "w+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remanin %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
					count++;
				
				}
				else
				{
					
					f = fopen("scheduler.log", "w+");	
   					fprintf(f,"At time %d process %d stopped arr %d total %d remanin %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   					fclose(f);
					enqueue(readyQueue,runningProcess);
				
				}
				
				
				
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
		if(runningFlag==0 && !isEmpty(readyPriorityQueue))
		{
			runningFlag=1;
			runningProcess=dequeuePriority(readyPriorityQueue);
			//first time enter the scheduler need forking
			if(runningProcess.remain==runningProcess.process.runningtime)//&& runningProcess.pid==-1)
			{
				
				f = fopen("scheduler.log", "w+");
				
   				fprintf(f,"At time %d process %d started arr %d total %d remanin %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				
   				fclose(f);
				int pid1=fork();
				if(pid1==-1)
					printf("EEEEEEEEEEE");
				if(pid1==0)//child
				{
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					char string[40];
					char now[20];
					char remaining[20];
				// convert 123 to string [buf]
					itoa(x, now, 20);
					itoa(runningProcess.remain,remaining,20);
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
				
				f = fopen("scheduler.log", "w+");	
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
					f = fopen("scheduler.log", "w+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remanin %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
					count++;
				
				}
				else
				{
					
					f = fopen("scheduler.log", "w+");	
   					fprintf(f,"At time %d process %d stopped arr %d total %d remanin %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   					fclose(f);
					enqueue(readyQueue,runningProcess);
				
				}
				
				
				
				}
				
			}
		
		
		}
	
	
	
	}



}

int main(int argc, char *argv[])
{
    initClk();

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    destroyClk(true);
}
