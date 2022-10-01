#include "main.h"
#include "colorprint.h"
#include "pinfo.h"

// parse the file in /proc/pid/stat, /proc/pid/status (easier) and /proc/pid/exe
// /proc/pid/exe => actual path name of the executed process 
// /proc/pid/stat => stats of the program
// /proc/pid/status => status of the program (for virtual memory)

/*
Official man pages states that we should read in order %d %s and %c (stat)
%d -> process ID (PID)
%s -> process name 
%c -> Running status
%d -> Parent PID
*/

/*
Official man pages states that we should find the field VmSize for this (status)
*/

void pinfo(char *tokens[]) 
{
    // Name of the executable path
    char*exe = malloc(PINFO_BUFFER_SIZE);strcpy(exe, "defunct ");

    /* STEP 1 : Open stat file */
    // This is the name of the stat file 
    char*add = malloc(PINFO_BUFFER_SIZE);

    strcpy(add,"/proc/");
    strcat(add,tokens[1]);
    strcat(add,"/stat");

    // Opens /proc/pid/stat
    FILE *reader = fopen(add, "r");
    if (reader == NULL) 
    {
        printRed();
        printf("\nACCESS DENIED to the process : Either the process does not exist or it has been terminated earlier\n");
        printWhite();
        return;
    }
    
    // Get pid , execution path and status code from the file 
    int pid, parentpid;
    char exec_name[PINFO_BUFFER_SIZE],st_code;
    fscanf(reader, " %d %s %c %d", &pid,exec_name,&st_code,&parentpid);

    free(add);

    /* STEP 2 : Open exe file*/
    char*add2 = malloc(PINFO_BUFFER_SIZE);
    strcpy(add2,"/proc/");
    strcat(add2,tokens[1]);
    strcat(add2,"/exe");
    
    int t = readlink(add2, exe, PINFO_BUFFER_SIZE);
    if (t < 0) 
    {
        strcat(exe, exec_name);
    } 
    else 
    {
        exe[t] = '\0';
    }

    free(add2);

    /* STEP 3 : Open status file */
    char*add3 = malloc(PINFO_BUFFER_SIZE);
    strcpy(add3,"/proc/");
    strcat(add3,tokens[1]);
    strcat(add3,"/status");

    fclose(reader);
    FILE*newreader = fopen(add3, "r");
    if (newreader == NULL) 
    {
        printRed();
        printf("\nACCESS DENIED to the process : Either the process does not exist or it has been terminated earlier\n");
        printWhite();
        return;
    }

    free(add3);

    char*word = malloc(PINFO_BUFFER_SIZE);

    while (fscanf(newreader, " %502s", word) != EOF) // Search for the VmSize field 
    {
        if (strcmp(word, "VmSize:") == 0) 
        {
            fscanf(newreader, " %502s", word);
            break;
        }
    }
    fclose(newreader);
    /* ALL STEPS DONE , NOW PRINT ON TERMINAL */
    printYellow();
    printf("Stats Obtained Successfully! Here are these:\n\n");
    printCyan();
    printf("PID : %d\n", pid);
    printf("Process Status : ");
    if(parentpid == pid)
    {
        printf("+");
    }
    printf("%c\n",st_code);
    printf("Memory : %s\n", word);
    printf("Executable Path : %s\n", exe);
    printWhite();
    printf("\n");

    free(word);
    return;
}


