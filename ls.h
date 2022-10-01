// This module will handle the 
#ifndef LS_H_
#define LS_H_

#define SIZE_OF_NAME 1002
#define SIZE_OF_PERMISSION 103
#define SIZE_OF_MONTH 10 
// Performs the ls command. Returns nothing 
// Inputs : token count , tokens and the home directory 
// Current directory can be taken care of by using the getcwd() function 
void ls(char *tokens[], int no, const char *curr_dir, const char *home_dir);


#endif