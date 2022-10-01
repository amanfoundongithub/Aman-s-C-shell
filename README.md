# Name : Aman Raj <br>
Compile the code as : 
```
>>> make
```
Run the code as : 
```
>>> ./shell
```
# Specifications Implemented Till Now
<strong> Till now , only Specifications 1,2 and 3 have been implemented.</strong>
# Purpose Of Each File 
1. <strong>main.c</strong> : The main file that runs the shell. Headers (neccessary for main.c as well as other files) have been included in main.h
- cd , echo and pwd are implemented in main.c file. 
- definition of the functions in break_line.h are in main.c 
- Signal handling (from previous Assignment) is also done in main.c file.

2. <strong>break_line.h</strong> : Contains prototypes of various functions that are needed to break/parse a line and do desired operations.

2. <strong>colorprint.c</strong> : The file which contains functions for coloring the output. Prototypes are in colorprint.h file. 

3. <strong>ls.c</strong> : Contains manual implementation of ls command of Linux. Prototype is in ls.h file. 

4. <strong>discovery.c</strong> : Contains manual implementation of discovery command (user defined). The prototypes of these are in discovery.h file.

4. <strong>pinfo.c</strong> : Contains manual implementation of pinfo command (user defined). The prototypes of these are in pinfo.h file.

4. <strong>history.c</strong> : Contains manual implementation of history command (user defined) and function to create history entry. The prototypes of these are in history.h file.

4. <strong>process.c</strong> : Contains functions for dealing with foreground and background processes. The prototypes of these are in process.h file.

4. <strong>job.c</strong> : Contains functions for dealing with the jobs in background. The job.h contains typedef(s) and definitions of the terms , as well as prototypes of the functions. Adding , removing , finding and printing jobs are included in this file. 

4. <strong>fg.c</strong> : Contains functions for dealing with fg command. The fg.h contains the prototype(s) of the fg function for executing "fg" on command line. 

4. <strong>bg.c</strong> : Contains functions for dealing with bg command. The bg.h contains the prototype(s) of the fg function for executing "bg" on command line. 

- Signals (Ctrl + C , Ctrl + Z and Ctrl + D) are handled in main.c file. 


- Few text files are also present as well as unneccesary directories. These were utilized for testing the commands and the shell. They have NO roles in the ``shell`` executable file.

- Autocomplete has been implemented and if there is a single file , it will complete the sentence and on pressing enter , you will get the command executed. If it is multiple files , then you will enter full from the list and after pressing enter TWO times , it will execute the command.

<h2>Few Important Points</h2>
- The maximum number of jobs is assumed to be 10002 for the entire shell. It is defined as MAX_NUM_OF_JOBS in the job.h file
