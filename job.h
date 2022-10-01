#ifndef JOBS_H_
#define JOBS_H_


// PID of the Job 
typedef int job_pid;

// ID of the Job (1,2,3,...)
typedef int job_id;

// Count of the number of jobs
typedef int count_jobs;


typedef struct _job_node{
    job_pid pid;
    char * process_name;
    job_id id;
} job_node;

// Add a node to the given linked list 
void AddNode(job_pid pid,char* process_name);

// Remove a node from the given list 
// Based  on : 
// 1. PID -> var = 'P'
// 2.  ID -> var = 'I'
void RemoveNode(int param,char var);

// Find a node based on : 
// 1. PID of the process (set var = 'P')
// 2. ID  of the process (set var = 'I')
int FindNode(int param,char var);

// Returns job status based on the job pid
char ReturnJobStatus(job_pid pid);

// Prints all the jobs 
void Print_Jobs();
#endif