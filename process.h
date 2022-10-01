#ifndef PROCESS_H_
#define PROCESS_H_


#define SIZE_OF_PROCESS 60

#define SIZE_OF_IT 1004

// We have to create the process to execute either in background or in the foreground 
// This functions takes the tokens (char *tokens[]) as input,
// Number of tokens (int count) and
// Flag to see if the process should run in background or foreground 
void create_process(int count,char*tokens[],bool flag);
#endif