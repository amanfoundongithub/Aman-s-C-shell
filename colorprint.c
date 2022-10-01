#include "colorprint.h"
#include <stdio.h>

void printBlue()
{
    printf("\033[0;34m");
}

void printGreen()
{
    printf("\033[0;32m");
}

void printRed()
{
    printf("\033[0;31m");
}

void printWhite()
{
    printf("\033[0;37m");
}

void printYellow()
{
    printf("\033[0;33m");
}

void printBlack()
{
    printf("\033[0;30m");
}

void printCyan()
{
    printf("\033[0;36m");
}

void printMagenta()
{
    printf("\x1B[35m");
}