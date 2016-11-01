#include "sfish.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

char Ucolor[128];
char Mcolor[128];
int userFlag = 1;
int machineFlag = 1;
char* prevDirectory;

 #define RED     "\033[22;31"
 #define GREEN   "\033[22;32"
 #define YELLOW  "\033[22;33"
 #define BLUE    "\033[22;34"
 #define MAGENTA "\033[22;35"
 #define CYAN    "\033[22;36"
 #define WHITE   "\033[22;37"
 #define BLACK   "\033[22;40"


void setColor(char* setting, char* color,  char* bold)
{
    char* user = getenv("USER");
    char machine[128];
    gethostname(machine, sizeof machine);

    if(strcmp(setting,"user")==0)
    {
        if (strcmp(color,"red")==0) strcpy(Ucolor, RED);
        else if (strcmp(color,"green")==0) strcpy(Ucolor, GREEN);
        else if (strcmp(color,"yellow")==0) strcpy(Ucolor, YELLOW);
        else if (strcmp(color,"blue")==0) strcpy(Ucolor, BLUE);
        else if (strcmp(color,"magenta")==0) strcpy(Ucolor, MAGENTA);
        else if (strcmp(color,"cyan")==0) strcpy(Ucolor, CYAN);  
        else if (strcmp(color,"white")==0) strcpy(Ucolor, WHITE); 
        else if (strcmp(color,"black")==0) strcpy(Ucolor, BLACK); 
        else
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }

        if(strcmp(bold,"1")==0) strcat(Ucolor, ";1m");
        else if(strcmp(bold,"0")==0) strcat(Ucolor, "m");
        else
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }
        strcat(Ucolor, user); 
        strcat(Ucolor, "\033[0m");
    }
    else if (strcmp(setting,"machine")==0)
    {
        if (strcmp(color,"red")==0) strcpy(Mcolor, RED);
        else if (strcmp(color,"green")==0) strcpy(Mcolor, GREEN);
        else if (strcmp(color,"yellow")==0) strcpy(Mcolor, YELLOW);
        else if (strcmp(color,"blue")==0) strcpy(Mcolor, BLUE);
        else if (strcmp(color,"magenta")==0) strcpy(Mcolor, MAGENTA);
        else if (strcmp(color,"cyan")==0) strcpy(Mcolor, CYAN);  
        else if (strcmp(color,"white")==0) strcpy(Mcolor, WHITE); 
        else if (strcmp(color,"black")==0) strcpy(Mcolor, BLACK);   
        else
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        } 

        if(strcmp(bold,"1")==0) strcat(Mcolor, ";1m");
        else if(strcmp(bold,"0")==0) strcat(Mcolor, "m");
        else
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }
        strcat(Mcolor, machine);
        strcat(Mcolor, "\033[0m");   
    }
        return;
}


const char* sfish(char* buffer)
{
    //char buffy[128];
    char cwd[128];
    //gethostname(buffy, sizeof buffy);
    getcwd(cwd, sizeof(cwd));
    //char user = getenv("USER");

    if(strcmp(cwd,getenv("HOME")) == 0) strcpy(cwd,"~");
    if(userFlag == 1 && machineFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s%s%s", "sfish-", Ucolor, "@", Mcolor, ":", "[", cwd, "]> ");
        return buffer;
    }
    else if (userFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s", "sfish-", Ucolor, ":", "[", cwd, "]> ");
        return buffer;
    }

    else if(machineFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s", "sfish-", Mcolor, ":", "[", cwd, "]> ");
        return buffer;
    }

    else{

        snprintf(buffer, 1024, "%s%s%s%s", "sfish:", "[", cwd, "]> ");
        return buffer;
    }
  
}

void printHelp()
{
    printf("%s\n", "cd [-L|[-P [-e]] [-@]] [dir]");
    printf("%s\n", "chclr [SETTING] [COLOR] [BOLD]");
    printf("%s\n", "chmpt [SETTING] [TOGGLE]");
    printf("%s\n", "exit [n]");
    printf("%s\n", "help [-dms] [pattern ...] ");
    printf("%s\n", "pwd Prints the absolute path of the current working directory.");
}


void chmpt(char* setting, char* toggle)
{
    if(strcmp(setting,"user") == 0)
    {
        if(strcmp(toggle,"0") == 0) userFlag = 0;
        if(strcmp(toggle,"1") == 0) userFlag = 1;
        else
            errno = EINVAL;
            return;
    }
    if(strcmp(setting,"machine") == 0)
    {
        if(strcmp(toggle,"0") == 0) machineFlag = 0;
        if(strcmp(toggle,"1") == 0) machineFlag = 1;
        else
            errno = EINVAL;
            return;
    }
    else
        errno = EINVAL;
        printf("%s\n", "Invalid argument");
        return;

}

void parse(char* cmd)
{
    char buffer[1028];
    char *first;
    char *second;
    char *third;
    char *fourth;

    first = strtok(cmd, " ");
    if (first == NULL) return;

    //Check all cases with 1 arg only
    if(strcmp(first,"exit")==0) exit(3);
    if(strcmp(first,"pwd")==0) printf("%s\n", getcwd(buffer, sizeof buffer));
    if(strcmp(first,"help")==0) printHelp();

    //Check all cases with multiple args
   

    //Checks for the 3 cd cases
    if(strcmp(first,"cd") == 0)
    {
        second = strtok(NULL, " ");
        if(second == NULL)
        {
            prevDirectory = getcwd(buffer, sizeof buffer);
            chdir(getenv("HOME"));
            printf("%s\n", getenv("HOME"));
            return;
        }

        else if(strcmp(second, "-") == 0)
        {
            printf("%s\n", prevDirectory);
            //holdDirectory = getcwd(buffer3, sizeof buffer3);
            chdir(prevDirectory);
            return;
        }
        else if (second != NULL) //User typed cd and at least a second arg.
        {
            DIR* dir = opendir(second);

            if (dir) //The directory is there so we can switch to it.
            {
                char buffer2[1028];
                closedir(dir);
                prevDirectory = getcwd(buffer2, sizeof buffer2);
                chdir(second);
                printf("%s\n", prevDirectory);
                return;
            }
            else     //The directory doesn't exist.
            {
                errno = ENOENT;
                printf("%s\n", "No such file or directory");
                return;
            }
        }
        else //The only input was cd.
        {
            prevDirectory = getcwd(buffer, sizeof buffer);
            chdir(getenv("HOME"));
            printf("%s\n", getenv("HOME"));
            return;
        }
    }
    //Checks chpmt cases
    else if(strcmp(first,"chpmt") == 0)
    {
        second = strtok(NULL, " ");
        if(second == NULL) 
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }
        third = strtok(NULL, " ");
        if(third == NULL)
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }
        chmpt(second, third);
        return;
    }

    else if(strcmp(first,"chclr") == 0)
    {
        second = strtok(NULL, " "); 
        if(second == NULL)
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        } 
        third = strtok(NULL, " "); 
        if(third == NULL)
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }
        fourth = strtok(NULL, " ");

        if(fourth == NULL)
        {
            errno = EINVAL;
            printf("%s\n", "Invalid argument");
            return;
        }

        setColor(second, third, fourth);
        return;
    }
    else if(first != NULL)
    {
       printf("%s\n", "Command not found");
       errno = EINVAL;
       return; 
    }

    
}

int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.


    //Initially set up the home variable
    chdir(getenv("HOME"));
    char *cmd;
    //char *holdDirectory;
    prevDirectory = getenv("HOME");
    char buffer[1024];
    //set initial colors to white
    setColor("user", "white", "0");
    setColor("machine", "white", "0");

    while((cmd = readline((sfish(buffer)))) != NULL) {

        parse(cmd);

        //All your debug print statments should be surrounded by this #ifdef
        //block. Use the debug target in the makefile to run with these enabled.
        #ifdef DEBUG
        fprintf(stderr, "Length of command entered: %ld\n", strlen(cmd));
        #endif
        //You WILL lose points if your shell prints out garbage values.

    }

    //Don't forget to free allocated memory, and close file descriptors.
    free(cmd);
    //WE WILL CHECK VALGRIND!

    return EXIT_SUCCESS;
}
