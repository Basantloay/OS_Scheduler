#include "headers.h"
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
    char now[20];
    sprintf(now,"%d", x);
    strcpy((char *)shmaddr2, now);

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
		if(runningFlag==0 && !isEmpty(readyQueue))
		{x = getClk();
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
				
   				fprintf(f,"At time %d process %d started arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				
   				fclose(f);
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
					runningProcess.wait+=(runningProcess.lastStart-runningProcess.process.arrivaltime);
				}
			
			
			
			}
			else
			{runningProcess.lastStart=x;
				runningProcess.wait+=(runningProcess.lastStart-runningProcess.finish);
				
				f = fopen("scheduler.log", "a+");	
   				fprintf(f,"At time %d process %d resumed arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				
				
				kill(runningProcess.pid,SIGCONT); 
				sendCurrentRemain();
			}
		
		
		}
		else if (runningFlag==1)
		{	x=getClk();
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
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
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
				sendCurrentRemain();
				
				
					runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
					runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
					actualcount++;
				kill(runningProcess.pid,SIGSTOP);
				
				}
				else
					sendCurrentRemain();
				
			}
		if(actualcount>=count)
			break;
		
		}
	
	
	
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
				
				f = fopen("scheduler.log", "a+");
   				fprintf(f,"At time %d process %d started arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
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
					execl("./process.out", "process.out ",now2,remaining2,(char*)0);
					
					//sendCurrentRemain();
					exit(0);
				
				}
				else//parent
				{
					
					runningProcess.firstStart=x;
					runningProcess.lastStart=x;
					runningProcess.pid=pid2;
					runningProcess.wait+=(runningProcess.lastStart-runningProcess.process.arrivaltime);
				}
			
			
			
			}
			else
			{runningProcess.lastStart=x;
				runningProcess.wait+=(runningProcess.lastStart-runningProcess.finish);
				
				f = fopen("scheduler.log", "a+");	
   				fprintf(f,"At time %d process %d resumed arr %d total %d remain %d wait %d\n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait);
   				fclose(f);
				
				sendCurrentRemain();
				kill(runningProcess.pid,SIGCONT); 
				
			}
		
		
		}
		else if (runningFlag==1)
		{	x=getClk();
			if(readyPriorityQueue->front->priorityofqueue<runningProcess.remain)
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
					f = fopen("scheduler.log", "a+");	
   					fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   					fclose(f);
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
					sendCurrentRemain();
				
			}
		if(actualcount>=count)
			break;
		
		}
	
	
	
	}



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
    if (shmid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID = %d\n", shmid);
    shmaddr2 = shmat(shmid, (void *)0, 0);
    if (shmaddr2 == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
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
   	fprintf(f2,"%d	%d	%d",algorithmnum,quantum,count);
   	fclose(f2);
   	initClk();
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

void USRhandler(int signum)
{
	runningFlag=0;
	runningProcess.finish=getClk();
	runningProcess.remain-=(getClk()-runningProcess.lastStart);
	
	if(runningProcess.remain==0)
	{
	runningProcess.TA=runningProcess.finish-runningProcess.process.arrivaltime;
	runningProcess.WTA=(float)runningProcess.TA/(float)runningProcess.process.runningtime;
	f = fopen("scheduler.log", "a+");	
   	fprintf(f,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f \n", x,runningProcess.process.id,runningProcess.process.arrivaltime,runningProcess.totalTime,runningProcess.remain,runningProcess.wait,runningProcess.TA,runningProcess.WTA);
   	fclose(f);
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

