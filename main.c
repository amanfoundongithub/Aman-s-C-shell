#include "main.h"
#include "colorprint.h"
#include "ls.h"
#include "process.h"
#include "history.h"
#include "pinfo.h"
#include "discovery.h"
#include "break_line.h"
#include "job.h"
#include "fg.h"
#include "bg.h"
#include "sig.h"

char* Tab(char*current,char*currentDir);

// Define statements for the size of various things
#define COMMAND_SIZE 100    // size of command
#define MESSAGE_SIZE 10001  // size of the commands line
#define DIRECTORY_SIZE 1001 // directory size
#define NAME_SIZE 1001      // size of name
#define END_OF_STR '\0'     // end of the string
#define MAX_LENGTH 1002     // maximum length 

job_node jobs[MAX_NUM_OF_JOBS];

int id = 1;

bool interrupted = false;

// Important
// #define DEBUG

// Directories stored.
// Home => home directory. It is important to store for the ~ operation.
// current => current directory. It is the directory we are in currently.
// showDirectory => directory string which will be printed when pwd is called.
// prevAddress => Address of the previous command (for the - command of cd )
char current[DIRECTORY_SIZE], Home[DIRECTORY_SIZE], showDirectory[DIRECTORY_SIZE], prevAddress[DIRECTORY_SIZE];

char *line;

int original_input, originia_output;


// Prototype of showDirectoryUpdate() for some functions
void showDirectoryUpdate();
void remove_whitespace(char **s);

// Name which is going to be displayed in the prompt
char *sname;

// echo command is implemented below
void echo(char *token[], int count_token)
{
    int i = 1;
    while (i < count_token)
    {
        printf("%s ", token[i]);
        i++;
    }
    printf("\n");
}

// pwd command is implemented below
void pwd()
{
    printYellow();
    printf("The present working directory is : ");
    printCyan();
    int length = strlen(current);
    for (int i = 0; i < length - 1; i++)
    {
        printf("%c", current[i]);
    }
    printWhite();
}

// cd command is implemented below
void cd(char *tokens[], int token_count)
{
    if (token_count == 1) // this behaves same as cd ~ so we put "~" there
    {
        tokens[1] = malloc(4);
        strcpy(tokens[1], "~");
    }

    bool flag = false; // flag is a variable which will be used below

    // computes new address to locate the system based on user input
    char new_address[NAME_SIZE], restore[NAME_SIZE]; // restore is used to restore prevAddress if the command fails

    strcpy(restore, prevAddress); // For safety, we have saved the value of the original file

    // tokens[1] is the second argument given after cd
    if (tokens[1][0] == '/') // If absolute address is implemented by the user
    {
        strcpy(new_address, tokens[1]);
    }
    else if (tokens[1][0] == '~') // ~ will return to the home directory
    {
        strcpy(new_address, Home);
        strcat(new_address, tokens[1] + 1);
    }
    else
    {
        strcpy(new_address, current);                    // Relative address is given by the user so we compute absolute address
        if (new_address[strlen(new_address) - 1] != '/') // Add a new / symbol if it is not there in the in the address initially
        {
            strcat(new_address, "/");
        }
        strcat(new_address, tokens[1]); // new address = current address + / + tokens[1]. This is the absolute address
    }

    if (strcmp(tokens[1], "-") == 0) // use previous address here
    {
        if (strlen(prevAddress) == 0)
        {
            printRed();
            printf("cd: OLDPWD not set\n");
            printWhite();
            return;
        }
        flag = true;
        strcpy(new_address, prevAddress);
    }
    // copy the current address to previous address
    strcpy(prevAddress, current);

    // Absolute target address computed now we have to do change directory...

    // stats of directory
    struct stat dir_stats;

    // Fill the stats in the dir_stats
    int success = stat(new_address, &dir_stats);

    // Check if the given target is a directory
    if (success == 0 && (S_IFDIR & dir_stats.st_mode))
    {
        if (chdir(new_address) == -1)
        {
            strcpy(prevAddress, restore);
            printRed();
            printf("cd: %s: No such file or directory\n", tokens[1]);
            printWhite();
        }

        if (flag)
        {
            printf("%s\n", prevAddress);
        }
        strcpy(current, "");
        getcwd(current, DIRECTORY_SIZE); // Get the current working directory

        strcat(current, "/"); // appends / at the end of the current so that appending any other address becomes easier

        showDirectoryUpdate(); // update the directories accordingly
    }
    else
    {
        strcpy(prevAddress, restore);
        printRed();
        printf("cd: %s: Not a directory\n", tokens[1]);
        printWhite();
    }
}

// Updates the showDirectory so that we can show user what we have got so far
void showDirectoryUpdate()
{
    char end = '\0';
    // If length of current directory is less than home directory length , means we need to update the name
    if (strlen(current) < strlen(Home))
    {
        strcpy(showDirectory, current); // copies current to showDirectory
        if (showDirectory[strlen(showDirectory) - 1] == '/')
        {
            showDirectory[strlen(showDirectory) - 1] = end; // showDirectory will have a '\0' at the end of it
        }
        return;
    }

    int i = 0;
    for (; i < strlen(Home); i++) // iterate over home directory name
    {
        if (Home[i] - current[i]) // if some character of Home and current does not match, then we need to update showDirectory to current
        {
            strcpy(showDirectory, current); // updates showDirectory to current
            return;
        }
    }
    // If showDirectory is updated initially , then it should be equal to the home Dir
    strcpy(showDirectory, "~/");

    // add the part in the showDirectory which is in current but not in Home i.e. relative address
    strcat(showDirectory, current + strlen(Home));

    // make the end of the showDirectory to be '\0' not the / character
    if (showDirectory[strlen(showDirectory) - 1] == '/')
    {
        showDirectory[strlen(showDirectory) - 1] = end;
    }
    return;
}

// Initialize the given terminal by finding the name and starting allocation
void initialize()
{
    // Login and hostname.
    // Terminal will print <loginname@hostname:~> => <sname:~>
    char loginname[NAME_SIZE], hostname[NAME_SIZE];

    // Find the login name
    int success = getlogin_r(loginname, NAME_SIZE);

    // Find the hostname
    success = gethostname(hostname, NAME_SIZE);

    // Find the name to be shown to the user i.e. sname
    strcpy(sname, loginname);
    strcat(sname, "@");
    strcat(sname, hostname);

    // We see that the sname => loginname@username

    // Now we update our Home easily
    char *suc = getcwd(Home, DIRECTORY_SIZE); // Absolute address

    // Append the '/' symbol to the name
    strcat(Home, "/");

    // Update current directory to the value by the Home
    strcpy(current, Home);

    // Update previous directory to home directory
    strcpy(prevAddress, "");

    // Initial update to the current directory
    showDirectoryUpdate();

    original_input = dup(STDIN_FILENO);
    originia_output = dup(STDOUT_FILENO);

    SHELL_PID = getpid();
    return;
}

// Take decision on the basis of the tokens we obtain
void takedecision(char **tokens, int num, bool flag, int *pipe, char *oldcommand)
{
    if(leave == true)
    {
        leave = false;
        return;
    }
// printf("tokens[0] : %s\n",tokens[0]);
// If no tokens are found, exit from the program or abort the command
#ifdef DEBUG
    for (int i = 0; i < num; i++)
    {
        printf("%s\n", tokens[i]);
    }

#endif
    if (!num)
        return;

    // echo command is called
    if (strcmp(tokens[0], "echo") == 0)
    {
        echo(tokens, num);
        return;
    }

    // pwd command is called
    if (strcmp(tokens[0], "pwd") == 0)
    {
        pwd();
        printf("\n");
        return;
    }

    // cd command is called
    if (strcmp(tokens[0], "cd") == 0)
    {
        cd(tokens, num);
        return;
    }

    if (strcmp(tokens[0], "history") == 0)
    {
        if (num == 1) // Default value of the history
        {
            tokens[1] = strdup("10");
        }
        history(atoi(tokens[1]));
        return;
    }

    if (strcmp(tokens[0], "ls") == 0)
    {
        ls(tokens, num, current, Home);
        return;
    }

    if (strcmp(tokens[0], "pinfo") == 0)
    {
        if (num == 1)
        {
            char currentprocess[SIZE_OF_PROCESS];

            sprintf(currentprocess, "%d", getpid());

            tokens[1] = strdup(currentprocess);
        }

        pinfo(tokens);
        return;
    }

    if (strcmp(tokens[0], "discover") == 0)
    {
        discovery(tokens, num, Home);
        return;
    }
    if (strcmp(tokens[0], "clear") == 0)
    {
        printf("\e[1;1H\e[2J");
        return;
    }
    if (strcmp(tokens[0], "jobs") == 0)
    {
        Print_Jobs(tokens, num);
        return;
    }
    if (strcmp(tokens[0], "fg") == 0)
    {
        fg(tokens, num);
        return;
    }
    if (strcmp(tokens[0], "bg") == 0)
    {
        bg(tokens, num);
        return;
    }
    if (strcmp(tokens[0], "sig") == 0)
    {
        sig(tokens, num);
        return;
    }
    // Special command for termination of the C shell
    if (strcmp(tokens[0], "exit") == 0)
    {
        int status;
        struct dirent *dir_stuff;

        DIR *iterator = opendir("/proc");

        if (iterator == NULL)
        {
            exit(EXIT_SUCCESS);
        }

        FILE *reader;
        while ((dir_stuff = readdir(iterator)) != NULL)
        {
            if (isdigit(dir_stuff->d_name[0]) != 0)
            {
                char *add = malloc(PINFO_BUFFER_SIZE);

                strcpy(add, "/proc/");
                strcat(add, dir_stuff->d_name);
                strcat(add, "/stat");

                reader = fopen(add, "r");

                if (!reader)
                {
                    continue;
                }

                int pid, parent_pid;
                char state, name[PINFO_BUFFER_SIZE];

                fscanf(reader, "%d %s %c %d", &pid, name, &state, &parent_pid);

                if (parent_pid == (int)getpid())
                {
                    kill(pid, SIGKILL);
                }

                fclose(reader);
            }
        }
        closedir(iterator);

        while (waitpid(-1, &status, WNOHANG) > 0);

        exit(EXIT_SUCCESS);
        return;
    }
    create_process(num, tokens, flag);
}

// Main implementation function to parse the command line
void get_to_work(char *line)
{
    // Add command to the history
    add_to_history(line);

    // line2 will be used for tokenising line, command is a pointer to iterate through the tokens.
    char line2[MESSAGE_SIZE], *command, line3[MESSAGE_SIZE];

    int count = 0, i = 0;

    // copy line to line2
    strcpy(line2, line);

    // copy line to line3
    strcpy(line3, line);

    // tokenise line using it
    command = strtok(line, ";&");

    // count number of tokens in the line i.e. number of different commands we get from it.
    while (command)
    {
        count++;
        command = strtok(NULL, ";&");
    }

    // If no tokens are obtained ,  we can exit the program
    if (count == 0)
        return;

    // create an array of commands obtained from the tokens.
    char *commands[count + 1];

    // now tokenise line2 and store all the commands in the array for use
    commands[0] = strtok(line2, ";&");

    // tokenise till we encounter an empty string or tokenising has finished.
    while (commands[i] && strlen(commands[i]))
    {
        commands[++i] = strtok(NULL, ";&");
    }
    bool flag, morethanone = false;
    // Iterate over the commands and take decisions based on them, passed to the takedecision function

    for (int j = 0; j < count; j++)
    {
        // printf("command[%d] : %s\n",j,commands[j]);
        flag = false;
        if (line3[strlen(commands[j]) + (commands[j] - line2)] == '&')
        {
            flag = true;
        }

        // remove whitespaces
        remove_whitespace(&commands[j]);

        // Checks for pipe

        ValidatePipe(commands[j], flag);

        // Fix Input
        close(STDIN_FILENO);
        dup2(original_input, STDIN_FILENO);
        // close(original_input);
        close(STDOUT_FILENO);
        dup2(originia_output, STDOUT_FILENO);
        // close(originia_output);
    }

    return;
}

void remove_whitespace(char **s)
{
    char *p = *s; // copy of pointer to the string
    char end = '\0';
    // Make the string free from space from the starting
    for (int i = 0; p[i] != '\0'; i++)
    {
        if (p[i] == ' ' || p[i] == '\t' || p[i] == '\n')
        {
            (*s)++; // the pointer is incremented as long as the first non space character is encountered
        }
        else
        {
            break;
        }
    }

    // now remove space from the last by replacing the characters with NULL characters
    int length = strlen(p) - 1;
    for (int i = length; i >= 0; i--)
    {
        if (p[i] == '\n' || p[i] == ' ' || p[i] == '\t')
        {
            p[i] = end;
        }
        else
        {
            break;
        }
    }
    return;
    // Since we have passed a double pointer, so we see that the pointer to the array will get updated.
    // So changes to pointer line (*s) will be seen.
}

// Interrupt generated when a child process has been completed
void kill_child(int signal_number)
{
    // Signal by the child that it has been completed
    if (signal_number == SIGCHLD)
    {
        int status, count = 0;
        struct dirent *dir_stuff;

        DIR *iterator = opendir("/proc");

        if (iterator == NULL)
        {
            exit(EXIT_SUCCESS);
        }

        FILE *reader;

        char *names[SIZE_OF_NAME];
        int pids[SIZE_OF_NAME];

        while ((dir_stuff = readdir(iterator)) != NULL)
        {
            if (isdigit(dir_stuff->d_name[0]) != 0)
            {
                char *add = malloc(PINFO_BUFFER_SIZE);

                strcpy(add, "/proc/");
                strcat(add, dir_stuff->d_name);
                strcat(add, "/stat");

                reader = fopen(add, "r");

                if (!reader)
                {
                    continue;
                }

                int pid, parent_pid;
                char state, name[PINFO_BUFFER_SIZE];

                fscanf(reader, "%d %s %c %d", &pid, name, &state, &parent_pid);

                if (parent_pid == (int)getpid())
                {
                    pids[count] = pid;
                    names[count++] = strdup(name);
                }

                fclose(reader);
            }
        }
        closedir(iterator);

        int src;
        int prinflag;
        while ((src = waitpid(-1, &status, WNOHANG)) > 0)
        {
            char reason[SIZE_OF_PERMISSION];

            if (WIFEXITED(status))
            {
                prinflag = 1;
                sprintf(reason, "normally with exit code %d\n", WIFEXITED(status));
            }

            else if (WIFSIGNALED(status))
            {
                prinflag = 2;
                sprintf(reason, "due to the signal code %d\n", WIFSIGNALED(status));
            }

            else
            {
                prinflag = 3;
                sprintf(reason, "due to some other reasons\n");
            }

            char name[SIZE_OF_NAME];

            int i = 0;
            for (; i < count; i++)
            {
                if (pids[i] == src)
                {
                    break;
                }
            }
            if (prinflag == 1)
                printCyan();
            if (prinflag == 2)
                printMagenta();
            if (prinflag == 3)
                printYellow();
            printf("\nProcess %s with PID : %d has exited %s", names[i], src, reason);
            interrupted = true;
            printWhite();
        }
    }
    return;
}

// Exits the program by killing all children of the main process
void exit_here(int signal_number)
{
    // SIGINT is the signal to end the whole shell (generated by Ctrl + C)
    if (signal_number == SIGINT)
    {
        int status;
        struct dirent *dir_stuff;

        DIR *iterator = opendir("/proc");

        if (iterator == NULL)
        {
            exit(EXIT_SUCCESS);
        }

        FILE *reader;
        while ((dir_stuff = readdir(iterator)) != NULL)
        {
            if (isdigit(dir_stuff->d_name[0]) == 1)
            {
                char *add = malloc(PINFO_BUFFER_SIZE);

                strcpy(add, "/proc/");
                strcat(add, dir_stuff->d_name);
                strcat(add, "/stat");

                reader = fopen(add, "r");

                if (!reader)
                {
                    continue;
                }

                int pid, parent_pid;
                char state, name[PINFO_BUFFER_SIZE];

                fscanf(reader, "%d %s %c %d", &pid, name, &state, &parent_pid);

                if (parent_pid == (int)getpid())
                {
                    kill(pid, SIGKILL);
                }

                fclose(reader);
            }
        }
        closedir(iterator);

        while (waitpid(-1, &status, WNOHANG) > 0)
            ;

        exit(EXIT_SUCCESS);
    }
}

// Handles Ctrl + C request 
void CtrlCHandler(int signum)
{
    int pid = getpid();

    if (pid < 0)
    {
        printRed();
        printf("FATAL ERROR : Unable to get the PID of the Parent Process.\n");
        printWhite();
        return;
    }
    else
    {
        if (pid != SHELL_PID)
        {
            RemoveNode(pid, 'P');
            exit(EXIT_FAILURE);
        }
        printf("\n");
        fflush(stdout);

        return;
    }
}

// Handles Ctrl + Z request 
void CtrlZHandler(int signum)
{
    int pid = getpid();

    if(pid != SHELL_PID)
    {
        printRed();
        printf("WARNING : Call this function only during shell.\n");
        printWhite();
        return;
    }

    if(curr_job.pid != 0)
    {
        AddNode(curr_job.pid,curr_job.process_name);
        
        kill(curr_job.pid,SIGTSTP);
    }
    
    printf("\n");
    return;

}

/* CTRL + D */
void die(const char *s) 
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}
/* CTRL + D */

// To get line using low level I/O 
void GetLine()
{
    setbuf(stdout,NULL);
    char c;
    int pt = 0;
    enableRawMode();
    if(leave == true)
    {
        leave = false;
        pt = strlen(line);
        goto king;
    }
    memset(line,'\0',MESSAGE_SIZE);
    king : 
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10)
                break;
            else if (c == 27)
            {
                char buf[3];
                buf[2] = 0;
                if (read(STDIN_FILENO, buf, 2) == 2)
                { // length of escape code
                    
                }
            }
            else if (c == 127)
            { // backspace
                if (pt > 0)
                {
                    if (line[pt - 1] == 9)
                    {
                        for (int i = 0; i < 7; i++)
                        {
                            printf("\b");
                        }
                    }
                    line[--pt] = '\0';
                    printf("\b \b");
                }
            }
            else if (c == 9)
            {
                success = false;
                char iterator[MAX_LENGTH],*s = iterator,*p = s;
                char* line2 = strdup(line);
                s = strtok(line2," \n\t");
                p = s;
                while(s != NULL)
                {
                    p = s;
                    s = strtok(NULL," \t\n");
                }
                if(p == NULL)
                {
                    p = strdup("");
                }
                char* lol =  Tab(p,".");
                if(lol != NULL)
                {
                    char*temp = strdup(current + strlen(Home) + strlen(showDirectory) - 2);
                    char printed[MAX_LENGTH];
                    strcpy(printed,lol + strlen(p));
                    strcat(line,printed);
                    printf("%s",lol + strlen(p));
                }

                if(leave == true) // leave the terminal 
                {
                    if(line2)
                    {
                        free(line2);
                    }
                    return;
                }
                if (success == false)
                {
                    line[pt++] = c;
                }
                if(line2)
                {
                    free(line2);
                }
                leave = false;
            }
            else if (c == 4)
            {
                printf("\n");
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("%d\n", c);
            }
        }
        else
        {
            line[pt++] = c;
            printf("%c", c);
        }
    }
    printf("\n");
    disableRawMode();
}

void get_to_work(char *);
// Main function for running the shell
signed main(void)
{
    leave = false;

    // Initialize the current job 
    curr_job.pid = 0;
    curr_job.process_name = NULL;

    // Clears the entire screen for our new shell
    printf("\e[1;1H\e[2J");

    // Allocates memory to the global variable sname.
    // malloc is important
    sname = malloc(NAME_SIZE);

    // Execute this routine if Ctrl + C is reached.
    signal(SIGINT, CtrlCHandler);

    // Execute this routine if Ctrl + Z is reached.
    signal(SIGTSTP, CtrlZHandler);

    // If we recieve a signal to kill a child , we will process the kill_child function
    // Setting signal for killing child process (background process finished) to function kill_child
    signal(SIGCHLD, kill_child);

    // If we recieve a signal to abort the program , we will kill the program
    // We have to kill all child process(es) also
    // Setting the signal to the function exit_here

    line = malloc(MESSAGE_SIZE); // line is the command line that is obtained on command line

    // Initialize the system by filling in the values of directory arrays as well as sname
    initialize();

    // The shell runs in the infinite loops
    while (true)
    {
    rerol:
        interrupted = false;
        // fflush(stdin);
        // fflush(stdout);
        /* BLOCK TO PRINT PROMPT BEGINS */
        printf("<");
        printGreen();
        printf("%s", sname);
        printWhite();
        printf(":");
        printBlue();
        printf("%s", showDirectory);
        printWhite();
        printf(">");
        /* BLOCK TO PRINT PROMPT ENDS  */

        //fgets(line, MESSAGE_SIZE, stdin); // reads the entire line from stdin
        GetLine();

        if (interrupted == true)
        {
            goto rerol;
        }

        if (*line && line[strlen(line) - 1] == '\n') // make the line a string by ending it with NULL character instead of '\n'
        {
            line[strlen(line) - 1] = END_OF_STR;
        }

        remove_whitespace(&line); // Extract the command by removing the intial and final space

        get_to_work(line); // This will be the main function , which will ask shell to get to work
    }
    free(line);
    return 0;
}

// Validates a pipe and creates one too
void ValidatePipe(char *token, bool background)
{
    int count_of_pipe = 0; // pipee

    int iterator = 0; // i

    while (token[iterator] != '\0')
    {
        if (token[iterator] == '|')
        {
            count_of_pipe++;
        }
        ++iterator;
    }

    // printf("pipee  : %d\n",count_of_pipe);
    // If we have no pipes , then do not handle the pipes
    if (count_of_pipe == 0)
    {
        // printf("token before Redirect : %s\n",token);
        Redirect(token, background, NULL);
        return;
    }

    // If both ends of the pipe are not there , then we output an error
    if (token[0] == '|' || token[iterator - 1] == '|')
    {
        printRed();
        printf("FATAL ERROR : Pipe does not have both the ends\n");
        printWhite();
        return;
    }

    // Now divide the commands on the basis of the | delimiter to get the total number of commands in between
    char *tokens[MAX_NO_OF_PIPES]; // commands

    int count = 0; // n

    char *iterator1 = strtok(token, "|");

    while (iterator1)
    {
        tokens[count++] = strdup(iterator1);
        iterator1 = strtok(NULL, "|");
    }

    for (int j = 0; j < count - 1; j++) // i
    {
        // Create a pipe.
        // pipe[1] bytes can be read by pipe[0]
        int curr_pipe[2];
        int success = pipe(curr_pipe);

        if (success == -1)
        {
            printRed();
            printf("PIPE ERROR : Unable to open a pipe\n");
            printWhite();
            return;
        }

        dup2(curr_pipe[1], STDOUT_FILENO);
        close(curr_pipe[1]);

        Redirect(tokens[j], background, curr_pipe);

        dup2(originia_output, STDOUT_FILENO);

        dup2(curr_pipe[0], STDIN_FILENO);
        close(curr_pipe[0]);
        free(tokens[j]);
    }

    Redirect(tokens[count - 1], background, NULL);

    return;
}

int tokenize(char *token, char *string, char *tokens[])
{
    bool found = true;
    int length_of_token = strlen(token), length_of_string = strlen(string);
    int count = 0;
    int start = 0, i;
    for (i = 0; i < length_of_string - length_of_token + 1;)
    {
        found = true;
        int j = 0;
        for (; j < length_of_token; j++)
        {
            if (token[j] != string[i + j])
            {
                found = false;
                break;
            }
        }
        if (found == true)
        {
            // last char has just been matched
            int k = i;
            for (; k < i + length_of_token; k++)
                string[k] = '\0';
            tokens[count] = strdup(string + start);
            count++;
            tokens[count] = strdup(token);
            count++;
            start = i + length_of_token;
            i += length_of_token;
        }
        else
            i++;
    }
    tokens[count] = strdup(string + start);
    count++;
    return count;
}

void Redirect(char *input, bool background, int *pipe)
{
    bool to_execute = true;
    char *tokens[MAX_NO_OF_PIPES], *input2 = strdup(input);

    tokens[0] = strtok(input, " \t\n");

    int iterator = 0; // num_tokens

    while (tokens[iterator] != NULL)
    {
        ++iterator;
        tokens[iterator] = strtok(NULL, " \t\n");
    }

    // printf("Value of iterator after tokenize : %d\n",iterator);
    if (!iterator)
    {
        return;
    }

    char tokens_append[MAX_SIZE][MAX_LENGTH];

    int k = 0; // n

    //  >>
    int i = 0;
    // printf("Value of iterator after tokenize : %d\n",iterator);
    while (i < iterator)
    {
        char *newtokens[MAX_SIZE];
        int length = tokenize(">>", tokens[i], newtokens); // c
        int j = 0;
        while (j < length)
        {
            if (strlen(newtokens[j]) == 0)
            {
                free(newtokens[j]);
                j++;
                continue;
            }
            strcpy(tokens_append[k++], newtokens[j]);
            free(newtokens[j]);
            j++;
        }
        i++;
    }

    // k -> n
    // >
    char tokens_append_out[MAX_SIZE][MAX_LENGTH];
    iterator = k;
    // printf("Value of iterator after tokenize : %d\n",iterator);
    i = 0;
    k = 0;
    while (i < iterator)
    {
        char *newtokens[MAX_SIZE];
        if (strcmp(tokens_append[i], ">>") == 0)
        {
            strcpy(tokens_append_out[k++], tokens_append[i]);
            i++;
            continue;
        }
        int length = tokenize(">", tokens_append[i], newtokens);
        int j = 0;
        while (j < length)
        {
            if (strlen(newtokens[j]) == 0)
            {
                free(newtokens[j]);
                j++;
                continue;
            }
            strcpy(tokens_append_out[k++], newtokens[j]);
            free(newtokens[j]);
            j++;
        }
        i++;
    }


    char tokens_final[MAX_SIZE][MAX_LENGTH];
    iterator = k;
    i = 0;
    k = 0;

    while (i < iterator)
    {
        char *newtokens[MAX_SIZE];
        int length = tokenize("<", tokens_append_out[i], newtokens);
        int j = 0;
        while (j < length)
        {
            if (strlen(newtokens[j]) == 0)
            {
                free(newtokens[j]);
                j++;
                continue;
            }
            strcpy(tokens_final[k++], newtokens[j]);
            free(newtokens[j]);
            j++;
        }
        i++;
    }

    char *command_tokens[MAX_NO_OF_PIPES];

    int total_words = 0; // num_word_command

    i = 0;
    while (i < k)
    {
        if (strcmp(tokens_final[i], "<") == 0 || strcmp(tokens_final[i], ">") == 0 || strcmp(tokens_final[i], ">>") == 0)
        {
            if (i == k - 1 || tokens_final[i + 1] == NULL)
            {
                printRed();
                printf("FATAL ERROR : Unexpected Token After %s\n", tokens_final[i]);
                // to_execute = false;
                return;
            }
            changeInput(tokens_final[i], tokens_final[i + 1]);
            i += 2;
            continue;
        }
        else
        {
            command_tokens[total_words++] = strdup(tokens_final[i]);
        }
        i++;
    }

    takedecision(command_tokens, total_words, background, pipe, input2);
    int clear = 0;
    for (; clear < total_words; clear++)
    {
        free(command_tokens[clear]);
    }
}

void changeInput(char *token, char *file)
{
    if (strcmp(token, ">") == 0)
    {
        int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

        if (fd < 0)
        {
            printRed();
            printf("FATAL ERROR : Unable to write to the file\n");
            printWhite();
            return;
        }
        else
        {
            dup2(fd, STDOUT_FILENO);
        }
        return;
    }
    else if (strcmp(token, ">>") == 0)
    {
        int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (fd < 0)
        {
            printRed();
            printf("FATAL ERROR : Unable to create/write the desired file\n");
            printWhite();
            return;
        }
        dup2(fd, STDOUT_FILENO);
        return;
    }
    int fd = open(file, O_RDONLY);
    if (fd < 0)
    {
        printRed();
        printf("FATAL ERROR : Unable to read the desired file\n");
        printWhite();
        return;
    }
    dup2(fd, STDIN_FILENO);
    return;
}


// TAB IMPLEMENTATION 
#define NUMBER 100

void Tab_Handler(char * target , char* curr_dir, int* count , char*list[],int len_target);


// Implements the main tab function 
char* Tab(char*current,char*currDir)
{
    char*list[NUMBER];
    int count = 0;

    int length = strlen(current);

    Tab_Handler(current,currDir,&count,list,length);

    if(count > 1)
    {
        printf("\n");
        for(int i = 0 ; i < count ; i++)
        {
            printf("%s\n",list[i]);
            free(list[i]);
            list[i] = NULL;
        }
        leave = true;
        /* BLOCK TO PRINT PROMPT BEGINS */
        printf("<");
        printGreen();
        printf("%s", sname);
        printWhite();
        printf(":");
        printBlue();
        printf("%s", showDirectory);
        printWhite();
        printf(">");
        /* BLOCK TO PRINT PROMPT ENDS  */
        printf("%s",line);
        GetLine();
        success = true;
        return NULL;
    }

    else if(count == 1)
    {
        success = true;
        char* t = strdup(list[0]);
        return t;
    }

    else 
    {
        success = false;
        return NULL;
    }
}

bool IsPrefix(char*str1,char*str2,int n)
{
    for(int i = 0 ; i < n ; i++)
    {
        if(str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}

void Tab_Handler(char * target , char* curr_dir, int* count , char*list[],int len_target)
{
    bool dflag = true;
    bool fflag = false;

    DIR* dir = opendir(curr_dir);

    if(dir == NULL)
    {
        return;
    }

    struct dirent*dir_stuff;

    // Iterate over the current directory 
    while ((dir_stuff = readdir(dir)) != NULL)
    {
        // currentaddress (relative address)
        char currentaddress[DISCOVER_COMMAND_MAX];

        strcpy(currentaddress,curr_dir);

        if(currentaddress[strlen(currentaddress) - 1] != '/')
        {
            strcat(currentaddress,"/");
        }

        strcat(currentaddress,dir_stuff->d_name);

        // current file stats
        struct stat dirstats;

        int s = stat(currentaddress,&dirstats);

        if(s==-1)
        {
            continue;
        }

        if(strcmp("..",dir_stuff->d_name) == 0 || strcmp(".",dir_stuff->d_name) == 0)
        {
            continue;
        }

        if(strcmp(curr_dir,".") == 0)
        {
            strcpy(currentaddress,dir_stuff->d_name);
        }
        //  Don't fall for the . and .. 
        if(IsPrefix(target,dir_stuff->d_name,len_target) == true)
        {
            list[*count] = strdup(currentaddress);
            (*count)++;
            continue;
        }

        // Now check if the given thing is a directory or a regular file 
        struct stat tempstats;
        int st = stat(currentaddress,&tempstats);
        if(st == -1)
        {
            continue;
        }
        // Directory means we again call recursion 
        if((S_IFDIR & tempstats.st_mode))
        {
            Tab_Handler(target,currentaddress,count,list,len_target);
        }
        else 
        {
            continue;
        }
        
    }
    closedir(dir);
    free(dir_stuff);
    return;
}

