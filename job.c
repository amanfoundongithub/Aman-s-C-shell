#include "main.h"
#include "colorprint.h"
#include "job.h"

extern job_node jobs[MAX_NUM_OF_JOBS];

extern int id;


void Sort_Jobs()
{
    job_node jb;
    for(int i = 0 ; i < id - 1; i++)
    {
        for(int j = i + 1 ; j < id - 1 ; j++)
        {
            if(strcmp(jobs[j].process_name,jobs[i].process_name) < 0)
            {
                jb = jobs[j];
                jobs[j] = jobs[i];
                jobs[i] = jb;
            }
        }
    }
    return;
}

// Add a node to the given linked list 
void AddNode(job_pid pid,char* process_name)
{
    jobs[id - 1].id = id;
    jobs[id - 1].pid = pid;
    jobs[id - 1].process_name = strdup(process_name);
    id++;
    Sort_Jobs();
    //printf("Done adding to the list: %d %d %s\n",jobs[id - 2].id,jobs[id - 2].pid,jobs[id - 2].process_name);
    return;
}

// Remove a node from the given list 
// For PID , var == 'P'
void RemoveNode(int param,char var)
{
    for(int i = 0 ; i < id - 1; i++)
    {
        if(var == 'P')
        {
            if(jobs[i].pid == param)
            {
                jobs[id - 1].pid = -1;
                jobs[id - 1].id  = -1;
                free(jobs[id - 1].process_name);
                jobs[id - 1].process_name = NULL;
                return;
            }
        }

        else if(var == 'I')
        {
            if(jobs[i].id == param)
            {
                jobs[id - 1].pid = -1;
                jobs[id - 1].id  = -1;
                free(jobs[id - 1].process_name);
                jobs[id - 1].process_name = NULL;
                return;
            }
        }
    }
    return;
}


// Find a node based on : 
// 1. PID of the process (set var = 'P')
// 2. ID  of the process (set var = 'I')
// Return index in the array 
// -1 for unsucccessful search 
int FindNode(int param,char var)
{
    for(int i = 0 ; i < id - 1; i++)
    {
        if(var == 'P')
        {
            if(jobs[i].pid == param)
            {
                return i;
            }
        }

        else if(var == 'I')
        {
            if(jobs[i].id == param)
            {
                return i;
            }
        }
    }
    return -1;
}

// Returns job status based on the pid 
char ReturnJobStatus(job_pid pid)
{
    if(id == 1)
    {
        return '\0';
    }
    if(pid == -1)
    {
        return '\0';
    }

    char path[1002];

    sprintf(path,"/proc/%d/stat",pid);

    FILE* statusfile = fopen(path,"r");

    if(statusfile == NULL)
    {
        return '\0';
    }   

    int pids;
    char exec_name[1002],st_code;
    fscanf(statusfile, "%d %s %c", &pids,exec_name,&st_code);

    return st_code;
}

void Print_Jobs(char*tokens[],int count)
{
    bool s_flag = false;
    bool r_flag = false;

    if(count == 1)
    {
        s_flag = true;
        r_flag = true;
    }

    else 
    {
        for(int j = 1 ; j < count ; j++)
        {
            if (tokens[j][0] != '-')
            {
                printRed();
                printf("FATAL ERROR : Invalid command. Aborted\n");
                printWhite();
                return;
            }

            for (int i = 0; tokens[j][i] != '\0'; i++)
            {
                if (tokens[j][i] == 'r')
                {
                    r_flag = true;
                }
                else if (tokens[j][i] == 's')
                {
                    s_flag = true;
                }
                else 
                {
                    printRed();
                    printf("FATAL ERROR : Invalid flag provided\n");
                    printWhite();
                    return;
                }
            }
        }
    }

    if(r_flag == false && s_flag == false)
    {
        s_flag = true;
        r_flag = true;
    }

    for(int i = 0 ; i < id - 1; i++)
    {
        if(jobs[i].id == -1)
        {
            continue;
        }

        char status = ReturnJobStatus(jobs[i].pid);

        if(status == '\0')
        {
            continue;
        }
        
        printCyan();
        printf("[%d] ",jobs[i].id);
        printYellow();

        if(r_flag == true && (status == 'S' || status == 'R'))
        {
            printf("Running ");
        }
        else if(s_flag == true && status == 'T')
        {
            printf("Stopped ");
        }

        printGreen();
        printf("%s ",jobs[i].process_name);
        printf("[%d]\n",jobs[i].pid);
    }

    return;
}