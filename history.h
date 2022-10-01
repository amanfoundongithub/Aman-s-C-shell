#ifndef HISTORY_H_

#define HISTORY_H_

// adds a command to the history of the commands 
void add_to_history(char *line);

// Main history command for the linux shell 
void history(int n);

#endif