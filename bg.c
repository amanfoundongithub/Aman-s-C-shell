#include "main.h"
#include "colorprint.h"
#include "job.h"

extern job_node jobs[MAX_NUM_OF_JOBS];

extern int id;

void bg(char*tokens[],int count)
{
    int status;
    // Do somethin
    if(count == 1)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , insufficient parameter(s) provided. Aborted\n");
        printWhite();
        return;
    }

    else if(count > 2)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , extra parameter(s) provided. Aborted\n");
        printWhite();
        return;
    }

    // Get the id of the job
    job_id id ;

    if((id = atoi(tokens[1])) <= 0)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , Job ID must be a positive integer.Aborted\n");
        printWhite();
        return;
    }

    int job = FindNode(id,'I'); // Index of the job in the array

    if(job == -1)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , Job ID does not exist. Aborted\n");
        printWhite();
        return;
    }

    /* EXECUTION BLOCK BEGINS HERE */

    // Ignore these signals during the execution of the commands
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);

    job_pid pid = jobs[job].pid;

    char* name = strdup(jobs[job].process_name);

    // Alert user that we have started our work
    printYellow();
    printf("SUCCESS! Process %s with PID : %d is running in background ...\n",name,pid);
    printWhite();

    kill(pid,SIGCONT); // continue the process 

    // Put back the default signals
    signal(SIGTTIN,SIG_DFL);
    signal(SIGTTOU,SIG_DFL);

    /* EXECUTION BLOCK ENDS HERE */
    return;
}