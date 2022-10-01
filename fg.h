#ifndef FG_H_
#define FG_H_
#include "job.h"


#define MY_STDIN 0
// Executes the fg command
// Requires the job id as input 
void fg(char*tokens[],int count);

#endif