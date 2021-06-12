#include "headers.h"
#include <string.h>


void clearResources(int);

void readFile()
{

}


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    char c,file_name[25];
    char buffer[100];
    int count = 0;
    FILE *fp1;
    strcpy(file_name,"./processes.txt");


    /////////////OPEN THE FILE////////

    fp1 = fopen(file_name, "r"); // read mode

    if (fp1 == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    ////////////////////////////////

    //find number of lines in file
    for (c = getc(fp1); c != EOF; c = getc(fp1))
    {
        if (c == '\n') // Increment count if this character is a newline
        {
            count = count + 1;
        }
    }  

    //declare array of that size

    struct processData processArray[count-1];

    //populate array

    rewind(fp1);    

    fscanf(fp1, "%s %s %s %s",buffer,buffer,buffer,buffer); //Skip first line
    int i=0;    
    while (i < count-1)
    {
        fscanf(fp1, "%d %d %d %d" ,  &processArray[i].id,&processArray[i].arrivaltime,&processArray[i].runningtime,&processArray[i].priority);
        printf(" Id: %d Arrival Time: %d Running Time: %d Priority: %d  \n",processArray[i].id,processArray[i].arrivaltime,processArray[i].runningtime,processArray[i].priority);
        i++;
    }

    fclose(fp1);


    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
        //default quanta = zero 
        int q = -1;
        int algo = -1;
        int pidC,pidS;
        //algo = argv[1];
        //if (argc > 2)
        //{
        //    q = argv[2];
        //}     
		while (true)
    	{
			printf("\n1 ===> FCFS");
		    printf("\n2 ===> SJF");
		    printf("\n3 ===> HPF");
		    printf("\n4 ===> SRTN");
		    printf("\n5 ===> RR");
		    printf("\nEnter The Number Of Scheduling Algorithm :");
		    
		    scanf("%d", &algo);
		    if(algo>5 || algo<1)
		    	printf("\n    Invalid Input for Number of Scheduling Algorithm !!!");
		    else 
		    	break;
    	}

        while (algo==5 )
        {
            printf("\nEnter Quantum Number Q For RR algorithm:");
            scanf("%d", &q);
            if(q>0)
            	break;
            else
            	printf("\n    Invalid Input for Quantum Q !!!");
        }
   

    // 3. Initiate and create the scheduler and clock processes.

    pidC = fork();
    if(pidC ==0)
    {
        execl("./clk.out", "clk.out", NULL);
        exit(0);
    }

     pidS = fork();
     if(pidS ==0)
     {
     	char algostr[20];
     	sprintf(algostr,"%d", algo);
     	char qstr[20];
     	sprintf(qstr,"%d", q);
     	char countstr[20];
     	sprintf(countstr,"%d", count);
     	
         execl("./scheduler.out", "scheduler.out", algostr,qstr,countstr,NULL);
         printf("hena");
         exit(0);
     }


    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function. 
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    int x = getClk();
    // 6. Send the information to the scheduler at the appropriate time.

    key_t key_id;
    int msgq_id;
    struct msgbuff buff;

    key_id = ftok("keyfile", 65);    
    
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    buff.mtype = 7;

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    i = 0;
    x = getClk();
    int send_val;

    while(true)
    {
        x = getClk();
        while(processArray[i].arrivaltime == x )//&& pidS==0)
        {
            printf("Current Time is %d\n", x);
            buff.d = processArray[i];
            send_val = msgsnd(msgq_id, &buff, sizeof(buff.d), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Errror in send");
            i++;
        }

    }

    // 7. Clear clock resources
    destroyClk(true);


}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
