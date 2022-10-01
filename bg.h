#ifndef BG_H_
#define BG_H_
#include "job.h"


#define MY_STDIN_BG 0
// Executes the bg command
// Requires the job id as input 
void bg(char*tokens[],int count);

#endif