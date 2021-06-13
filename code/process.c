#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int current;
int main(int agrc, char *argv[])
{
    initClk();
	key_t key_id_process;
    int msgq_id;
    struct msgbuff_process buff;
/*
    key_id = ftok("keyfile", 67);    
    
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    buff.mtype = 5;
    */
    int shmid;
    key_id_process = ftok("keyfile", 67);    
    shmid = shmget(key_id_process, 4096, IPC_CREAT | 0644);
    if (shmid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID = %d\n", shmid);
    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime =atoi(argv[1]);
	current=atoi(argv[2]);
	FILE *f3 = fopen("test.log", "a+");	
   			fprintf(f3,"last check %d 	%d \n",remainingtime,current);
   	
   	fclose(f3);
    while (remainingtime > 0)
    {
        /*int rec_val= msgrcv(msgq_id, &buff, (sizeof(buff.remain)+sizeof(buff.current)), 0, IPC_NOWAIT);
    	if(rec_val!=-1)
    		{
    		remainingtime=buff.remain;
    		current=buff.current;
    		*/
   	char remain[20];
   	strcpy(remain,(char *)shmaddr);
   	remainingtime=atoi(remain);
   	FILE *f3 = fopen("test.log", "a+");	
   			fprintf(f3,"process %d  \n",remainingtime);
   	
   	fclose(f3);}
    
	kill(getppid(),SIGUSR1);
    destroyClk(false);

    return 0;
}
