#include "main.h"
#include "process.h"
#include "colorprint.h"
#include "job.h"

extern job_node jobs[MAX_NUM_OF_JOBS];

extern int id;

#define MAX_SIZE 100005

void create_process(int count,char*tokens[],bool flag)
{
    char commandi[MAX_SIZE]; // original command 
    strcpy(commandi,tokens[0]);
    for(int i = 1 ; i < count ; i++)
    {
        strcat(commandi," ");
        strcat(commandi,tokens[i]);
    }

    // Find the command we wanted to execute 
    char command[SIZE_OF_PROCESS];
    strcpy(command,tokens[0]);

    // Make an array of tokens till length count + 1 (execvp ke liye)
    char* commands[count+1];

    commands[count] = NULL;
    for(int i = 0 ; i < count ; i++)
    {
        commands[i] = strdup(tokens[i]);
    }

    // Now fork 
    int pid = fork();

    if(pid < 0)
    {
        printRed();
        printf("FATAL ERROR : Unable to create child process\n");
        printWhite();
        return;
    }
    if(pid == 0)
    {
        if(flag == true)
        {
            setpgid(0,0);
        }

        curr_job.pid = pid;
        curr_job.process_name = strdup(commandi);
        curr_job.id = -1;
        int success = execvp(command,commands);
        if(success < 0)
        {
            printRed();
            printf("FATAL ERROR : Invalid command provided. Please provide a valid command\n");
            printWhite();
            exit(EXIT_FAILURE);
        }
        return;
    }

    if(flag == false)
    {
        waitpid(pid,NULL,WUNTRACED);
        AddNode(pid,commandi);
    }

    else
    {
        AddNode(pid,commandi);
        printf("PID : %d\n",pid);
    }
}

