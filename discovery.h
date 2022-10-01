#ifndef DISCOVER_H_
#define DISCOVER_H_

#define DISCOVER_COMMAND_MAX 1003

void discovery_handler(char* target,char* curr_directory, bool dflag , bool fflag);

void discovery(char*tokens[] , int num,char*homedir);


#endif