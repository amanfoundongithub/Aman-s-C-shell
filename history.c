#include "main.h"
#include "history.h"
#include "colorprint.h"

#define SIZE_OF_BUFFER 1003

#define MAX_NO_OF_LINES 103

void add_to_history(char tokens[])
{
    if (strlen(tokens) == 0)
        return;
    
    char temptokens[SIZE_OF_BUFFER];

    sprintf(temptokens,"%s\n",tokens);

    FILE *f_temp= fopen("/tmp/.shell_history", "a");
    fclose(f_temp);

    FILE* reader = fopen("/tmp/.shell_history", "r");

    char *lines[MAX_NO_OF_LINES];

    int n = 0;

    size_t len = 0;

    if (reader)
    {
        lines[0] = malloc(SIZE_OF_BUFFER);

        while(fgets(lines[n],SIZE_OF_BUFFER,reader) != NULL)
        {
            if (strcmp(lines[n], "\n") == 0)
            {
                continue;
            }
            lines[++n] = malloc(SIZE_OF_BUFFER);
        }
    }
    fclose(reader);

    FILE* writer = fopen("/tmp/.shell_history", "w");

    int i = n - 20;
    
    if(i < 0)
    {
        i = 0;
    }

    for (; i < n; i++)
    {
        fprintf(writer, "%s", lines[i]);
    }

    if (i == 0)
    {
        fprintf(writer, "%s", temptokens);
    }

    else if(strcmp(temptokens,lines[i-1]))
    {
        fprintf(writer,"%s",temptokens);
    }

    fclose(writer);

    for (i = 0; i <= n; i++)
    {
        free(lines[i]);
    }

    return;
}

void history(int no)
{
    FILE *f_temp = fopen("/tmp/.shell_history", "a");
    fclose(f_temp);

    FILE* reader = fopen("/tmp/.shell_history", "r");

    char *lines[MAX_NO_OF_LINES];
    int n = 0;

    if (reader)
    {
        lines[0] = malloc(SIZE_OF_BUFFER);
        while (fgets(lines[n],SIZE_OF_BUFFER,reader) != NULL)
        {
            if (strcmp(lines[n], "\n") != 0)
                lines[++n] = malloc(SIZE_OF_BUFFER);
        }
    }

    fclose(reader);

    int i = n - no; 

    if(i < 0)
    {
        i = 0;
    }

    for (; i < n; i++)
    {
        printf("%s\n", lines[i]);
        free(lines[i]);
    }

    return;
}