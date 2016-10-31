#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

extern char Ucolor[128];
extern char Mcolor[128];


void setColor(char* color, int colorFlag, int bold);


const char* sfish(int userFlag, int machineFlag, char* buffer);