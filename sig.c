#include "sig.h"
#include "main.h"
#include "colorprint.h"
#include "job.h"

extern job_node jobs[MAX_NUM_OF_JOBS];

extern int id;

void sig(char*tokens[],int count)
{
    if(count < 3)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , insufficient argument(s) provided. Aborted\n");
        printWhite();
        return;
    }
    else if(count > 3)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , more than sufficient argument(s) provided. Aborted\n");
        printWhite();
        return;
    }

    // Signal number
    signal_id sign = atoi(tokens[2]);

    // Job ID 
    job_id id = atoi(tokens[1]);

    if(sign <= 0)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , Signal Number Cannot Be A Non-Positive Integer. Aborted\n");
        printWhite();
        return;
    }

    if(id <= 0)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , Job ID Cannot Be A Non-Positive Integer. Aborted\n");
        printWhite();
        return;
    }

    int job_index = FindNode(id,'I');

    if(job_index == -1)
    {
        printRed();
        printf("FATAL ERROR : Invalid Input , Job ID Does Not Exist. Aborted\n");
        printWhite();
        return;
    }

    // Success message print karo 
    printYellow();
    printf("SUCCESS! Process %s with PID : %d has been signalled.\n",jobs[job_index].process_name,jobs[job_index].pid);
    printWhite();

    if(kill(jobs[job_index].pid,sign) < 0)
    {
        printRed();
        printf("FAILURE! Unable to signal the process (PID : %d)\n",jobs[job_index].pid);
        printWhite();
    }
    
    return;
}