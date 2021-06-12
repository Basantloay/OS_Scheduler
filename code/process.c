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
    while (remainingtime > 0)
    {
        int rec_val= msgrcv(msgq_id, &buff, (sizeof(buff.remain)+sizeof(buff.current)), 0, IPC_NOWAIT);
    	if(rec_val!=-1)
    		remainingtime=buff.remain;
    }
	//kill(getppid(),SIGUSR1);
    destroyClk(false);

    return 0;
}
