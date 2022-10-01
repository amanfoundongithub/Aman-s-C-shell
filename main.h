#ifndef MAIN_H_

#define MAIN_H_

// Standard Libraries From The C Compiler 
// I don't like to write this too many times 
// So I added them to this main.h header file which I will import always 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include "ctype.h"
#include <termios.h>
#include "job.h"

#define MAX_NUM_OF_JOBS 10002 // Maximum number of jobs

int SHELL_PID;

job_node curr_job;

bool leave;

bool success;

#endif