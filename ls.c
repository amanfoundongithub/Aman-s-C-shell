#include "main.h"
#include "ls.h"
#include "colorprint.h"

// Prints the details of the given file 
void detail_print(const char *add, char *name, int detail)
{
    struct stat data;

    int sucess = lstat(add,&data);
    if (sucess == -1)
    {
        printRed();
        printf("Error in retrieving file details\n");
        printWhite();
        return;
    }

    char permission[SIZE_OF_PERMISSION];

    if (data.st_mode & S_IXUSR)
    {
        printGreen();
    }
    if (data.st_mode & S_IFDIR)
    {
        strcpy(permission, "d");
        printBlue();
    }
    else
    {
        strcpy(permission, "-");
    }
    if (detail == false)
    {
        printf("%s\n", name);
        printWhite();
        return;
    }

    struct passwd *pws = getpwuid(data.st_uid);

    struct group *grp = getgrgid(data.st_gid);

    time_t l_m = data.st_mtime;
    struct tm last_mod;
    localtime_r(&l_m, &last_mod);

    int month = last_mod.tm_mon;

    int flags[] = {S_IRUSR, S_IWUSR, S_IXUSR,S_IRGRP, S_IWGRP, S_IXGRP,S_IROTH, S_IWOTH, S_IXOTH};

    for(int i = 0 ; i < 9 ; i++)
    {
        if(data.st_mode & flags[i])
        {
            if(i % 3 == 0)
            {
                strcat(permission,"r");
            }
            else if(i%3 == 1)
            {
                strcat(permission,"w");
            }
            else if(i%3 == 2)
            {
                strcat(permission,"x");
            }
        }
        else 
        {
            strcat(permission,"-");
        }
    }
    
    char monthname[SIZE_OF_MONTH];

    switch(month)
    {
        case 0: strcpy(monthname,"Jan");break;
        case 1: strcpy(monthname,"Feb");break;
        case 2: strcpy(monthname,"Mar");break;
        case 3: strcpy(monthname,"Apr");break;
        case 4: strcpy(monthname,"May");break;
        case 5: strcpy(monthname,"Jun");break;
        case 6: strcpy(monthname,"Jul");break;
        case 7: strcpy(monthname,"Aug");break;
        case 8: strcpy(monthname,"Sep");break;
        case 9: strcpy(monthname,"Oct");break;
        case 10: strcpy(monthname,"Nov");break;
        case 11: strcpy(monthname,"Dec");break;
        default: strcpy(monthname,"Err");
    }

    printf("%s%5d%10s%10s%10ld %s %02d %02d:%02d %s\n", permission, (int) data.st_nlink, pws->pw_name, grp->gr_name, data.st_size,
           monthname, last_mod.tm_mday,
           last_mod.tm_hour , last_mod.tm_min , name);
    printWhite();
    return;
}

void print_ls_data(char *location,bool hidden,bool details,bool file, char *namefile,bool detail)
{
    
    if (strlen(namefile) == 0 && detail == true)
    {
        printf("%s :\n", namefile);
    }

    if (file == true)
    {
        detail_print(location, namefile, details);
        return;
    }

    DIR *dir = opendir(location);
    int total_directories = 0;
    while (readdir(dir))
    {
        total_directories++;
    }
    closedir(dir);
    
    char names[total_directories][SIZE_OF_NAME];
    dir = opendir(location);
    
    int count = 0 , total_blocks = 0;
    struct dirent *dir_stuff;
    while ((dir_stuff = readdir(dir)) != NULL)
    {
        strcpy(names[count++], dir_stuff->d_name);

        if (hidden == false && dir_stuff->d_name[0] == '.')
            continue;
        
        char element_address[SIZE_OF_NAME];
        strcpy(element_address, location);
        strcat(element_address, names[count - 1]);

        struct stat data;

        int suc = stat(element_address,&data);
        if (suc == -1)
        {
            printRed();
            printf("Unexpected Error While Retrieving Stats\n");
            printWhite();
            continue;
        }
        total_blocks += data.st_blocks;
    }

    free(dir_stuff);
    closedir(dir);

    char temp[SIZE_OF_NAME];

    int i = 0;
    for (; i < total_directories ; i++)
    {
        int j = i + 1;
        for (; j < total_directories ; j++)
        {
            if(strcmp(names[i],names[j]) <= 0)
            {
                continue;
            }
            strcpy(temp, names[j]);
            strcpy(names[j], names[i]);
            strcpy(names[i], temp);
        }
    }

    if (details == true)
    {
        printf("total %d\n", total_blocks/2);
    }

    for (int i = 0; i < total_directories ; i++)
    {
        if (names[i][0] == '.' && hidden == false)
        {
            continue;
        }

        char element_address[SIZE_OF_NAME];
        strcpy(element_address, location);
        strcat(element_address, names[i]);
        detail_print(element_address, names[i], details);
    }
    
    
}

void ls(char *tokens[], int no, const char *curr_dir, const char *home_dir)
{

    int directory_index[no + 1];
    int count_directory = 0;

    bool hidden = false;
    bool details = false;

    char location[SIZE_OF_NAME];

    for (int i = 0; i < no; i++)
    {
        if(i == 0)
        {
            continue;
        }
        if (tokens[i][0] == '-')
        {
            int j = 0;
            for (; j < strlen(tokens[i]); j++)
            {
                if(j == 0)
                {
                    continue;
                }
                
                if (tokens[i][j] == 'a')
                    hidden = true;
                else if (tokens[i][j] == 'l')
                    details = true;
                else
                {
                    printRed();
                    printf("ls: %c : not a flag\n",tokens[i][j]);
                    printWhite();
                    return;
                }
            }
        }
        else
        {
            directory_index[count_directory++] = i;
        }
    }

    bool detail = true;

    if (count_directory == 0)
    {
        detail = false;
        tokens[no] = strdup(".");
        count_directory = 1;
        directory_index[0] = no;
    }

    for (int j = 0; j < count_directory; j++)
    {
        int i = directory_index[j];
        if (tokens[i][0] == '/') 
        {
            strcpy(location, tokens[i]);
        } 
        else if (tokens[i][0] == '~') 
        {
            strcpy(location, home_dir);
            strcat(location, tokens[i] + 1);
        } 
        else 
        {
            strcpy(location, curr_dir);
            if (location[strlen(location) - 1] != '/')
            {
                strcat(location, "/");
            }
            strcat(location, tokens[i]);
        }

        struct stat stats_dir;
        int succ = stat(location,&stats_dir);

        if(succ == -1)
        {
            printRed();
            printf("ls: No such file or directory\n");
            printWhite();
            continue;
        }

        int flag = -1;
        if (S_IFDIR & stats_dir.st_mode)
        {
            if (location[strlen(location) - 1] != '/')
            {
                strcat(location, "/");
            }
            flag = false;
        }
        else if (S_IFREG & stats_dir.st_mode)
        {
            flag = true;
        }

        if(flag != -1)
        {
            print_ls_data(location, hidden, details, flag, tokens[i], detail);
        }
        else
        {
            printRed();
            printf("ls: %s: No such file or directory\n",location);
            printWhite();
        }
        printf("\n");
    }
}