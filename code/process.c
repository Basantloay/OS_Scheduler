#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int current;
int main(int agrc, char *argv[])
{
    initClk();
	key_t key_id;
    int msgq_id;
    struct msgbuff_process buff;

    key_id = ftok("keyfile", 67);    
    
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    buff.mtype = 5;
    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime =atoi(argv[1]);
	current=atoi(argv[2]);
	FILE *f3 = fopen("test.log", "a+");	
   			fprintf(f3,"last check %d 	%d \n",remainingtime,current);
   	
   	fclose(f3);
    while (remainingtime > 0)
    {
        int rec_val= msgrcv(msgq_id, &buff, (sizeof(buff.remain)+sizeof(buff.current)), 0, IPC_NOWAIT);
    	if(rec_val!=-1)
    		{
    		remainingtime=buff.remain;
    		current=buff.current;
    		FILE *f3 = fopen("test.log", "a+");	
   			fprintf(f3,"process %d 	%d \n",remainingtime,buff.current);
   	
   	fclose(f3);}
    }
	kill(getppid(),SIGUSR1);
    destroyClk(false);

    return 0;
}
