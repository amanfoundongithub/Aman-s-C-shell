#include "colorprint.h"
#include "main.h"
#include "discovery.h"
#include "ls.h"

// Recursive function that will print the answer 
void discovery_handler(char * target , char* curr_dir, bool dflag , bool fflag)
{
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

        //  Don't fall for the . and .. 
        if(strcmp(dir_stuff->d_name,"..") == 0 || strcmp(dir_stuff->d_name,".") == 0)
        {
            continue;
        }

        // Base case (printing case) for the recursion 
        if(strcmp(dir_stuff->d_name,target) == 0)
        {
            if(dflag == true)
            {
                if(S_IFDIR & dirstats.st_mode)
                {
                    printf("%s\n",currentaddress);
                }
            }
            if(fflag == true)
            {
                if(S_IFREG & dirstats.st_mode)
                {
                    printf("%s\n",currentaddress);
                }
            }
        }
        // Now check if the given thing is a directory or a regular file 
        struct stat tempstats;
        int st = stat(currentaddress,&tempstats);
        if(st == -1)
        {
            continue;
        }

        if(strlen(target) == 0)
        {
            if(dflag == true)
            {
                if(S_IFDIR & dirstats.st_mode)
                {
                    printf("%s\n",currentaddress);
                }
            }
            if(fflag == true)
            {
                if(S_IFREG & dirstats.st_mode)
                {
                    printf("%s\n",currentaddress);
                }
            }
        }
        // Directory means we again call recursion 
        if((S_IFDIR & tempstats.st_mode))
        {
            discovery_handler(target,currentaddress,dflag,fflag);
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

// Main function which will be called by the discover command
void discovery(char* tokens[] , int num,char* homedir)
{
    // flags for the -d , -f , file and directory
    bool dflag = false;
    bool fflag = false;

    // names of file and directory along with their pointers 
    char file[DISCOVER_COMMAND_MAX],*fileptr = NULL;

    char directory[DISCOVER_COMMAND_MAX],*dirptr = NULL;

    int dircount = 0;

    // Iterate over the tokens (0 is the command itself)
    for(int i = 1 ; i < num ; i++)
    {
        // Flag detected
        if(tokens[i][0] == '-')
        {
            for(int j = 1 ; j < strlen(tokens[i]) ; j++)
            {
                if(tokens[i][j] == 'd')
                {
                    dflag = true;
                }
                else if(tokens[i][j] == 'f')
                {
                    fflag = true;
                }
                else 
                {
                    printRed();
                    printf("FATAL ERROR : Invalid flag has been passed\n");
                    printWhite();
                    return;
                }
            }
        }
        else // It is either file or directory 
        {
            if(i == 1)
            {
                if(tokens[1][0] == '"')
                {
                    tokens[1][strlen(tokens[1]) - 1] = '\0';
                    tokens[1]++;
                }
                strcpy(directory,tokens[1]);
                dirptr = directory;
                dircount++;
            }
            else 
            {
                if(tokens[i][0] == '"')
                {
                    tokens[i][strlen(tokens[i]) - 1] = '\0';
                    tokens[i]++;
                }
                strcpy(file,tokens[i]);
                fileptr = file;
                dircount++;
            } 
        }
    }

    if(fileptr == NULL)
    {
        strcpy(file,"");
        fileptr = file;
    }

    if(dirptr == NULL)
    {
        strcpy(directory,".");
        dirptr = directory;
    }

    if(dirptr != NULL && dirptr[0] == '~')
    {
        char newdir[SIZE_OF_NAME];
        strcpy(newdir,homedir);
        strcat(newdir,dirptr+2);
        dirptr = newdir;
    }

    if(dircount == 0 || strlen(file) == 0)
    {
        if(dflag == false && fflag == false)
        {
            dflag = true;
            fflag = true;
        }
    }
    if(strlen(file) == 0)
    {
        printf("%s\n",directory);
    }

    if(dircount == 2 && (dflag == false && fflag == false))
    {
        dflag = true;
        fflag = true;
    }

    discovery_handler(fileptr,dirptr,dflag,fflag);
    return;
}


/*

gcc -g main.c ls.c process.c discovery.c history.c colorprint.c pinfo.c job.c fg.c bg.c sig.c tab.c

*/