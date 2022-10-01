#ifndef BREAK_LINE_H_

#define BREAK_LINE_H_

#define MAX_NO_OF_PIPES 1004
#define NOT_ALLOCATED -1
#define BUFF_SIZE_BREAK 1003
#define MAX_SIZE 108

void changeInputStream(char*,char*);

void changeInput(char*,char*);

void Redirect(char*,bool,int*);

void ValidatePipe(char*,bool);


#endif