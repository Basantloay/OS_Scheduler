#include "headers.h"
#include <math.h>
///////////////// Global data members \\\\\\\\\\\\\\

queue *readyQueue;
priorityqueue *readyPriorityQueue;

PCB runningProcess;
int runningFlag=0;//0 not running process, 1 otherwise
FILE *f;
int count;
int actualcount;
int quantum;
int algorithmnum;
key_t key_id;
int msgq_id;
struct msgbuff buff;
int x;
key_t key_id_process;
int msgq_id_process;
struct msgbuff_process buff_process;
int shmid;
void *shmaddr2;
 
void USRhandler(int signum);
double totalWTA=0;
long totalWaiting=0;
long totalRunning=0;
float utilization=0;

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
    		FILE *f3 = fopen("test.log", "a+");	
    		fprintf(f3,"\nRECEIVED ID : %d\n",receivedProcess.process.id);
    		fclose(f3);
    		receivedProcess.pid=-1;
    		receivedProcess.remain=buff.d.runningtime;
    		receivedProcess.firstStart=0;
    		receivedProcess.lastStart=0;
    		receivedProcess.finish=0;
    		receivedProcess.totalTime=buff.d.runningtime;
    		receivedProcess.wait=0;
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

void sendCurrentRemain()
{
	/*
	buff_process.current=x;
	buff_process.remain=runningProcess.remain;
	int send_val_process;
	 send_val_process= msgsnd(msgq_id_process, &buff_process, sizeof(buff_process.current)+sizeof(buff_process.remain), IPC_NOWAIT);
    if (send_val_process == -1)
    	perror("Errror in send");
    */
    char remain[20];
    int num=runningProcess.remain-(x-runningProcess.lastStart);
    sprintf(remain,"%d", num);
    strcpy((char *)shmaddr2, remain);

}

void sendRemain()
{
	/*
	buff_process.current=x;
	buff_process.remain=runningProcess.remain;
	int send_val_process;
	 send_val_process= msgsnd(msgq_id_process, &buff_process, sizeof(buff_process.current)+sizeof(buff_process.remain), IPC_NOWAIT);
    if (send_val_process == -1)
    	perror("Errror in send");
    */
    char remain[20];
    int num=runningProcess.remain;
    sprintf(remain,"%d", num);
    strcpy((char *)shmaddr2, remain);

}
//////////// Printing Finishing statement of process \\\\

void printingFinish()
{

	f = fopen("scheduler.log", "a+");
	if(fmodf(runningProcess.WTA,(float)1)==(float)0)
   		fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,(int)runningProcess.WTA);
  
   	else if(fmodf(runningProcess.WTA/0.1,1)==(float)0)
   		fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %0.1f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
  
   	else	
   		fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %0.2f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   	fclose(f);

}
//////////// Printing Performance of Scheduler \\\\\\\\\\

void printingPerformance()
{
	float avgWTA=totalWTA/(double)count;
	float avgWaiting=(float)totalWaiting/count;
	utilization=((float)totalRunning/x)*100;
	FILE *f4 = fopen("scheduler.perf", "a+");
	
	if(fmodf(utilization,(float)1)==(float)0)
   		fprintf(f4,"CPU utilization = %d %%\n",(int)utilization);
   	else if(fmodf(utilization/0.1,1)==(float)0)
   		fprintf(f4,"CPU utilization = %0.1f %%\n",utilization);
   	else
   		fprintf(f4,"CPU utilization = %0.2f %%\n",utilization);
   		
   	if(fmodf(avgWTA,(float)1)==(float)0)
   		fprintf(f4,"Avg WTA = %d\n",(int)avgWTA);
   	else if(fmodf(avgWTA/0.1,1)==(float)0)
   		fprintf(f4,"Avg WTA = %0.1f\n",avgWTA);
   	else 
   		fprintf(f4,"Avg WTA = %0.2f\n",avgWTA);
   
   	if(fmodf(avgWaiting,(float)1)==(float)0)
   		fprintf(f4,"Avg Waiting = %d\n",(int)avgWaiting);
   	else if(fmodf(avgWaiting/0.1,1)==(float)0)
   		fprintf(f4,"Avg Waiting = %0.1f\n",avgWaiting);
   	else
   		fprintf(f4,"Avg Waiting = %0.2f\n",avgWaiting);
   		
   	fclose(f4);
}
/////////////// Clean Resources \\\\\\\\\\\\\\\\\\\\\\\\\\

void cleanUp()
{
	msgctl(msgq_id,IPC_RMID,(struct msqid_ds *)0);
	shmdt(shmaddr2);
	shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
	//exit(0);

}

///////////////// Round-Robin RR Algorithm \\\\\\\\\\\\\\\

void RR()
{
FILE *f3 = fopen("test.log", "a+");	
   	fprintf(f3,"ENTERED RR \n");
   	
   	fclose(f3);
   	
	while(1)
	{
		
		receiveProcess();
		//f3 = fopen("test.log", "a+");	
   		//fprintf(f3,"b3d receive \n");
   		//fclose(f3);
   		x = getClk();
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
				
				
				int pid1=fork();
				if(pid1==-1)
					printf("EEEEEEEEEEE");
				else if(pid1==0)//child
				{
					
					
					char string[40];
					char now[20];
					char remaining[20];
				// convert 123 to string [buf]
					sprintf(now,"%d", x);
					
					sprintf(remaining,"%d",runningProcess.remain);
					execl("./process.out", "process.out ",now,remaining,(char*)0 );
					//sendCurrentRemain();
					exit(0);
				
				}
				else//parent
				{
					
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					runningProcess.pid=pid1;
					runningProcess.wait=(runningProcess.lastStart-runningProcess.process.arrivaltime);
					f = fopen("scheduler.log", "a+");
				
   				fprintf(f,"At time %d process %d started arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				
   				fclose(f);
				}
			
			
			
			}
			else
			{runningProcess.lastStart=x;
			runningProcess.wait+=(runningProcess.lastStart-runningProcess.finish);
				
				f = fopen("scheduler.log", "a+");	
   				fprintf(f,"At time %d process %d resumed arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				
				
				kill(runningProcess.pid,SIGCONT); 
				sendRemain();
			}
		
		
		}
		else if (runningFlag==1)
		{	//x=getClk();
			if((x-runningProcess.lastStart)>=quantum &&quantum<=runningProcess.remain)
			{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"corner case %d	%d \n",x,runningProcess.lastStart);
   	
   	fclose(f3);
				runningFlag=0;
				runningProcess.finish=x;
				runningProcess.remain-=(x-runningProcess.lastStart);
				sendCurrentRemain();
				
				if(runningProcess.remain<=0)
				{
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					totalWTA+=(double)runningProcess.WTA;
					totalWaiting+=(long)runningProcess.wait;
					totalRunning+=(long)runningProcess.totalTime;
					printingFinish();
					actualcount++;
				
				}
				else
				{
					
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d stopped arr %d total %d remain %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   					fclose(f);
					enqueue(readyQueue,runningProcess);
				
				}
				
				kill(runningProcess.pid,SIGSTOP);
				
				}
				else if(runningProcess.remain<quantum&&(x-runningProcess.lastStart)>=runningProcess.remain)
				{
					runningFlag=0;
				runningProcess.finish=x;
				runningProcess.remain-=(x-runningProcess.lastStart);
				sendRemain();
				
				
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					totalWTA+=(double)runningProcess.WTA;
					totalWaiting+=(long)runningProcess.wait;
					totalRunning+=(long)runningProcess.totalTime;
					printingFinish();
					actualcount++;
				kill(runningProcess.pid,SIGSTOP);
				
				}
				else
				
					sendRemain();
					
				
			}
		if(actualcount>=count)
			break;
		
		}
	printingPerformance();
	
	
	}

///////////////// Shortest Remaining Time Next SRTN Algorithm \\\\\\\\\\\\

void SRTN()
{
FILE *f3 = fopen("test.log", "a+");	
   	fprintf(f3,"ENTERED SRTN \n");
   	
   	fclose(f3);
   	
	while(1)
	{
		
		receiveProcess();
		if(runningFlag==0 && !isEmptyPriority(readyPriorityQueue))
		{x = getClk();
			runningFlag=1;
			runningProcess=dequeuePriority(readyPriorityQueue);
			//first time enter the scheduler need forking
			f3 = fopen("test.log", "a+");	
   			fprintf(f3,"ENTERED SRTN %d	%d 		%d \n",runningProcess.remain,runningProcess.process.runningtime,runningProcess.pid);
   	
   			fclose(f3);
   	
			if( runningProcess.pid==-1)
			{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"gowaaa SRTN %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
				
				
				int pid2=fork();
				if(pid2==-1)
					printf("EEEEEEEEEEE");
				else if(pid2==0)//child
				{
					char now2[20];
					char remaining2[20];
				// convert 123 to string [buf]
					sprintf(now2,"%d", x);
					
					sprintf(remaining2,"%d",runningProcess.remain);
					f3 = fopen("test.log", "a+");	
   	fprintf(f3,"executed process %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
					execl("./process.out", "process.out ",now2,remaining2,NULL);
					
					//sendCurrentRemain();
					//exit(0);
				
				}
				else//parent
				{
					
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					runningProcess.pid=pid2;
					runningProcess.wait+=(runningProcess.lastStart-runningProcess.process.arrivaltime);
					f = fopen("scheduler.log", "a+");
   				fprintf(f,"At time %d process %d started arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				}
			
			
			
			}
			else
			{
			kill(runningProcess.pid,SIGCONT);
			runningProcess.lastStart=x;
				runningProcess.wait+=(runningProcess.lastStart-runningProcess.finish);
				
				f = fopen("scheduler.log", "a+");	
   				fprintf(f,"At time %d process %d resumed arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				
				
				 
				sendCurrentRemain();
			}
		
		
		}
		else if (runningFlag==1)
		{	x=getClk();
			if(x-runningProcess.lastStart>runningProcess.remain||(!isEmptyPriority(readyPriorityQueue)&&readyPriorityQueue->front->priorityofqueue<(x-runningProcess.lastStart)))
			{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"corner case %d	%d \n",x,runningProcess.lastStart);
   	
   	fclose(f3);
				runningFlag=0;
				runningProcess.finish=x;
				runningProcess.remain-=(x-runningProcess.lastStart);
				sendCurrentRemain();
				
				if(runningProcess.remain<=0)
				{
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					totalWTA+=(double)runningProcess.WTA;
					totalWaiting+=(long)runningProcess.wait;
					totalRunning+=(long)runningProcess.totalTime;
					printingFinish();
					actualcount++;
				
				}
				else
				{
					
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d stopped arr %d total %d remain %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   					fclose(f);
					enqueuePriority(readyPriorityQueue,runningProcess,runningProcess.remain);
				
				}
				
				kill(runningProcess.pid,SIGSTOP);
				
				}
				
				else
				{	//runningProcess.remain-=(x-runningProcess.lastStart);
					sendCurrentRemain();}
				
			}
		if(actualcount>=count)
			break;
		
		}
	
	printingPerformance();
	
	}

/////////////////Shortest Job First SJF\\\\\\\\\\\\\\\\\

void SJF()
{
FILE *f3 = fopen("test.log", "a+");	
   	fprintf(f3,"ENTERED SJF \n");
   	
   	fclose(f3);
   	
	while(1)
	{
		
		receiveProcess();
		if(runningFlag==0 && !isEmptyPriority(readyPriorityQueue))
		{x = getClk();
			runningFlag=1;
			runningProcess=dequeuePriority(readyPriorityQueue);
			//first time enter the scheduler need forking
			f3 = fopen("test.log", "a+");	
   			fprintf(f3,"ENTERED SJF %d	%d 		%d \n",runningProcess.remain,runningProcess.process.runningtime,runningProcess.pid);
   	
   			fclose(f3);
   	
			if( runningProcess.pid==-1)
			{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"gowaaa SJF %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
				
				
				int pid2=fork();
				if(pid2==-1)
					printf("EEEEEEEEEEE");
				else if(pid2==0)//child
				{
					char now2[20];
					char remaining2[20];
				// convert 123 to string [buf]
					sprintf(now2,"%d", x);
					
					sprintf(remaining2,"%d",runningProcess.remain);
					f3 = fopen("test.log", "a+");	
   	fprintf(f3,"executed process %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
					execl("./process.out", "process.out ",now2,remaining2,NULL);
					
					//sendCurrentRemain();
					//exit(0);
				
				}
				else//parent
				{
					
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					runningProcess.pid=pid2;
					runningProcess.wait+=(runningProcess.lastStart-runningProcess.process.arrivaltime);
					f = fopen("scheduler.log", "a+");
   				fprintf(f,"At time %d process %d started arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				}
			
			
			
			}
			
		
		
		}
		else if (runningFlag==1)
		{	x=getClk();
			
			if(x-runningProcess.lastStart>=runningProcess.remain)
				{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"corner case %d	%d \n",x,runningProcess.lastStart);
   	
   	fclose(f3);
				runningFlag=0;
				runningProcess.finish=x;
				runningProcess.remain-=(x-runningProcess.lastStart);
				sendCurrentRemain();
				
				
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					totalWTA+=(double)runningProcess.WTA;
					totalWaiting+=(long)runningProcess.wait;
					totalRunning+=(long)runningProcess.totalTime;
					printingFinish();
					actualcount++;
				
				
				
				kill(runningProcess.pid,SIGSTOP);
				
				}
				
				else
					{	
					sendCurrentRemain();}
				
			}
		if(actualcount>=count)
			break;
		
		}
	
	printingPerformance();
	
	}

///////////////// First Come First Serve FCFS\\\\\\\\\\\\\\\\\

void FCFS()
{
FILE *f3 = fopen("test.log", "a+");	
   	fprintf(f3,"ENTERED FCFS \n");
   	
   	fclose(f3);
   	
	while(1)
	{
		
		receiveProcess();
		if(runningFlag==0 && !isEmpty(readyQueue))
		{x = getClk();
			runningFlag=1;
			runningProcess=dequeue(readyQueue);
			//first time enter the scheduler need forking
			f3 = fopen("test.log", "a+");	
   			fprintf(f3,"ENTERED FCFS %d	%d 		%d \n",runningProcess.remain,runningProcess.process.runningtime,runningProcess.pid);
   	
   			fclose(f3);
   	
			if( runningProcess.pid==-1)
			{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"gowaaa FCFS %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
				
				
				int pid2=fork();
				if(pid2==-1)
					printf("EEEEEEEEEEE");
				else if(pid2==0)//child
				{
					char now2[20];
					char remaining2[20];
				// convert 123 to string [buf]
					sprintf(now2,"%d", x);
					
					sprintf(remaining2,"%d",runningProcess.remain);
					f3 = fopen("test.log", "a+");	
   	fprintf(f3,"executed process %d	%d \n",runningProcess.remain,runningProcess.process.runningtime);
   	
   	fclose(f3);
					execl("./process.out", "process.out ",now2,remaining2,NULL);
					
					//sendCurrentRemain();
					//exit(0);
				
				}
				else//parent
				{
					
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					runningProcess.pid=pid2;
					runningProcess.wait+=(runningProcess.lastStart-runningProcess.process.arrivaltime);
					f = fopen("scheduler.log", "a+");
   				fprintf(f,"At time %d process %d started arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				}
			
			
			
			}
			
		}
		else if (runningFlag==1)
		{	x=getClk();
			
			if(x-runningProcess.lastStart>=runningProcess.remain)
				{
				f3 = fopen("test.log", "a+");	
   	fprintf(f3,"corner case %d	%d \n",x,runningProcess.lastStart);
   	
   	fclose(f3);
				runningFlag=0;
				runningProcess.finish=x;
				runningProcess.remain-=(x-runningProcess.lastStart);
				sendCurrentRemain();
				
				
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					totalWTA+=(double)runningProcess.WTA;
					totalWaiting+=(long)runningProcess.wait;
					totalRunning+=(long)runningProcess.totalTime;
					f = fopen("scheduler.log", "a+");	
   					printingFinish();
					actualcount++;
				
				
				kill(runningProcess.pid,SIGSTOP);
				
				}
				
				else
				{	
					sendCurrentRemain();
				}
				
			}
		if(actualcount>=count)
			break;
		
		}
	
	printingPerformance();
	
	}

////////////////// Highest Priority First \\\\\\

void HPF()
{
	FILE *f3 = fopen("test.log", "a+");
	fprintf(f3, "ENTERED HPF \n");

	fclose(f3);

	while (1)
	{

		receiveProcess();
		if (runningFlag == 0 && !isEmptyPriority(readyPriorityQueue))
		{
			x = getClk();
			runningProcess = dequeuePriority(readyPriorityQueue);
			runningProcess.wait = x - runningProcess.process.arrivaltime;
			//first time enter the scheduler need forking
			f3 = fopen("test.log", "a+");
			fprintf(f3, "ENTERED HPF %d	%d 		%d \n", runningProcess.remain, runningProcess.process.runningtime, runningProcess.pid);

			fclose(f3);

			if (runningProcess.pid == -1)
			{

				int pid2 = fork();
				if (pid2 == -1)
				{
				}
				else if (pid2 == 0) //child
				{
					char now2[20];
					char remaining2[20];
					// convert 123 to string [buf]
					sprintf(now2, "%d", x);

					sprintf(remaining2, "%d", runningProcess.process.runningtime);
					f3 = fopen("test.log", "a+");
					fprintf(f3, "executed process %d	%d \n", runningProcess.remain, runningProcess.process.runningtime);

					fclose(f3);
					execl("./process.out", "process.out ", now2, remaining2, NULL);

					//sendCurrentRemain();
					exit(0);
				}
				else //parent
				{

					runningProcess.firstStart = x;
					runningProcess.lastStart = x;
					runningProcess.pid = pid2;
					runningProcess.wait += (runningProcess.lastStart - runningProcess.process.arrivaltime);
					runningFlag = 1;
					f = fopen("scheduler.log", "a+");
					fprintf(f, "At time %d process %d started arr %d total %d remain %d wait %d\n", x, runningProcess.process.id, runningProcess.process.arrivaltime, runningProcess.totalTime, runningProcess.remain, runningProcess.wait);
					fclose(f);
				}

				kill(runningProcess.pid,SIGSTOP);

			}
			else
			{
				runningProcess.lastStart = x;
				runningProcess.wait += (runningProcess.lastStart - runningProcess.finish);

				f = fopen("scheduler.log", "a+");
				fprintf(f, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", x, runningProcess.process.id, runningProcess.process.arrivaltime, runningProcess.totalTime, runningProcess.remain, runningProcess.wait);
				fclose(f);
				kill(runningProcess.pid, SIGCONT);
				runningFlag = 1;
			}
		}
		else if (runningFlag == 1)
		{
			x = getClk();
			if (!isEmptyPriority(readyPriorityQueue) || (readyPriorityQueue->front->priorityofqueue ==  runningProcess.process.priority))
			{
				f3 = fopen("test.log", "a+");
				fprintf(f3, "corner case %d	%d \n", x, runningProcess.process.priority);
				fclose(f3);
				runningFlag = 0;
				runningProcess.finish = x;
				runningProcess.remain -= (x - runningProcess.lastStart);
				sendCurrentRemain();

				if (runningProcess.remain <= 0)
				{
					runningProcess.TA = runningProcess.finish - runningProcess.process.arrivaltime;
					runningProcess.WTA = (float)runningProcess.TA / (float)runningProcess.process.runningtime;
					totalWTA += (double)runningProcess.WTA;
					totalWaiting += (long)runningProcess.wait;
					totalRunning += (long)runningProcess.totalTime;
					printingFinish();
					actualcount++;
				}

				else
				{	//runningProcess.remain-=(x-runningProcess.lastStart);
					sendCurrentRemain();}

				{

					f = fopen("scheduler.log", "a+");
					fprintf(f, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", x, runningProcess.process.id, runningProcess.process.arrivaltime, runningProcess.totalTime, runningProcess.remain, runningProcess.wait);
					fclose(f);
					enqueuePriority(readyPriorityQueue, runningProcess, runningProcess.process.priority);
				}

				kill(runningProcess.pid, SIGSTOP);
			}

			else
			{ //runningProcess.remain-=(x-runningProcess.lastStart);
				sendCurrentRemain();
			}
		if(actualcount>=count)
			break;

		}

	

		if (actualcount >= count)
			break;
	}
printingPerformance();
}

////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	signal(SIGUSR1,USRhandler);	
   	//printf("hena 2");
	readyQueue = (queue*)malloc(sizeof(queue));//for RR,FCFS
	readyPriorityQueue= (priorityqueue*)malloc(sizeof(priorityqueue));//for SJF,HPF,SRTN
	init(readyQueue);
	initPriority(readyPriorityQueue);
	key_id = ftok("keyfile", 65);    
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    key_id_process = ftok("keyfile", 67);    
    shmid = shmget(key_id_process, 4096, IPC_CREAT | 0644);
    
    printf("\nShared memory ID = %d\n", shmid);
    shmaddr2 = shmat(shmid, (void *)0, 0);
    
	//msgq_id_process = msgget(key_id_process, 0666 | IPC_CREAT);

	//buff_process.mtype = 5;
    buff.mtype = 7;
    
	algorithmnum=atoi(argv[1]);
	quantum=atoi(argv[2]);
	count=atoi(argv[3]);
	count=count-1;//TO BE TESTED
	actualcount=0;
	FILE *f2 = fopen("test.log", "a+");	
   	fprintf(f2,"ENTERED SCHEDULER \n");
   	fprintf(f2,"%d	%d	%d\n",algorithmnum,quantum,count);
   	fclose(f2);
   	initClk();
	if(algorithmnum==1)//FCFS
	{
	
		FCFS();
	
	}
	else if(algorithmnum==2)//SJF
	{
	
		SJF();
	}
	else if(algorithmnum==3)//HPF
	{
	
		HPF();
	}
	else if(algorithmnum==4)//SRTN
	{
		SRTN();
	
	}
	else if(algorithmnum==5)//RR
	{
		RR();
	}
	cleanUp();
    
    kill(getppid(),SIGUSR2);
    //exit(0);
    destroyClk(true);
}

void USRhandler(int signum)
{
	runningFlag=0;
	runningProcess.finish=getClk();
	runningProcess.remain-=(getClk()-runningProcess.lastStart);
	
	if(runningProcess.remain==0)
	{
	runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
	runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
	totalWTA+=(double)runningProcess.WTA;
	totalWaiting+=(long)runningProcess.wait;
	totalRunning+=(long)runningProcess.totalTime;
	printingFinish();
	count++;
				
	}
	else
	{
					
	f = fopen("scheduler.log", "a+");	
   	fprintf(f,"At time %d process %d stopped arr %d total %d remain %d wait %d \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
    fclose(f);
	enqueue(readyQueue,runningProcess);
				
				}
	signal(SIGUSR1,USRhandler);
}

