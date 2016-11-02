#include "sfish.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


#define RED     "\033[22;31"
#define GREEN   "\033[22;32"
#define YELLOW  "\033[22;33"
#define BLUE    "\033[22;34"
#define MAGENTA "\033[22;35"
#define CYAN    "\033[22;36"
#define WHITE   "\033[22;37"
#define BLACK   "\033[22;40"



void chmpt(char* setting, char* toggle)
{
    if(strcmp(setting,"user") == 0)
    {
        if(strcmp(toggle,"0") == 0) userFlag = 0;
        else if(strcmp(toggle,"1") == 0) userFlag = 1;
        else
            error();
            return;
    }
    else if(strcmp(setting,"machine") == 0)
    {
        if(strcmp(toggle,"0") == 0) machineFlag = 0;
        else if(strcmp(toggle,"1") == 0) machineFlag = 1;
        else
            error();
            return;
    }
    else
    {
        error();
        return;
    }
    prtValue = 0;  //SUCCESS!
    return;

}

void prt()
{   
    printf("%d\n", prtValue);
    prtValue = 0;
}


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
            error();
            return;
        }

        if(strcmp(bold,"1")==0) strcat(Ucolor, ";1m");
        else if(strcmp(bold,"0")==0) strcat(Ucolor, "m");
        else
        {
            error();
            return;
        }
        strcat(Ucolor, user); 
        strcat(Ucolor, "\033[0m");
        prtValue = 0;
        return;
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
            error();
            return;
        } 

        if(strcmp(bold,"1")==0) strcat(Mcolor, ";1m");
        else if(strcmp(bold,"0")==0) strcat(Mcolor, "m");
        else
        {
            error();
            return;
        }
        strcat(Mcolor, machine);
        strcat(Mcolor, "\033[0m");
        prtValue = 0;
        return;   
    }

    error();
    return;
}


void printHelp()
{
    printf("%s\n", "help");
    printf("\t%s\n\n", "Print a list of all builtin’s and their basic usage in a single column.");
    printf("%s\n", "exit");
    printf("\t%s\n\n", "Exits the shell.");
    printf("%s\n", "cd");
    printf("\t%s\n", "Changes the current working directory of the shell by using the chdir(2) system call");
    printf("\t%s\n", "cd - changes the working directory to the last directory the user was in.");
    printf("\t%s\n\n", "cd with no arguments goes to the user’s home directory.");
    printf("%s\n", "pwd");
    printf("\t%s\n\n", "Prints the current working directory.");
    printf("%s\n", "chpmt:  Change prompt settings");
    printf("\t%s\n\n", "Usage: chpmt SETTING TOGGLE");
    printf("\t%s\n", "Valid values for SETTING are:");
    printf("\t\t%s\n", "user: The user field in the prompt.");
    printf("\t\t%s\n\n", "machine: The context field in the prompt.");
    printf("\t%s\n", "Valid values for TOGGLE are:");
    printf("\t\t%s\n", "1: Enabled");
    printf("\t\t%s\n\n", "0: Disabled");
    printf("%s\n", "chclr:  Change prompt colors");
    printf("\t%s\n\n", "Usage: chclr SETTING COLOR BOLD");
    printf("\t%s\n", "Valid values for SETTING are:");
    printf("\t\t%s\n", "user: The user field in the prompt.");
    printf("\t\t%s\n\n", "machine: The context field in the prompt.");
    printf("\t%s\n", "Valid values for COLOR are:");
    printf("\t\t%s\n", "red: ANSI red.");
    printf("\t\t%s\n", "blue: ANSI blue.");
    printf("\t\t%s\n", "green: ANSI green.");
    printf("\t\t%s\n", "yellow: ANSI yellow.");
    printf("\t\t%s\n", "cyan: ANSI cyan.");
    printf("\t\t%s\n", "magenta: ANSI magenta.");
    printf("\t\t%s\n", "black: ANSI black.");
    printf("\t\t%s\n\n", "white: ANSI white.");
    printf("\t%s\n", "To toggle BOLD use:");
    printf("\t\t%s\n", "1: Enabled");
    printf("\t\t%s\n\n", "0: Disabled");
}


void builtins(char* cmd)
{
    char buffer[1028];
    char *first;
    char *second;
    char *third;
    char *fourth;
    
    first = strtok(cmd, " ");
    if (first == NULL) return;

    //Check all cases with 1 arg only
    else if(strcmp(first,"exit")==0) exit(3);
    else if(strcmp(first,"pwd")==0) 
    {  
        pid_t pid = fork();
        if (pid == 0)  printf("%s\n", getcwd(buffer, sizeof buffer));
        else 
        {
            int status;
            wait(&status);
            exit(0);
        }
    }

    else if(strcmp(first,"help")==0) 
    {   
        pid_t pid = fork();
        if (pid == 0) printHelp();
        else 
        {
            int status;
            wait(&status);
            exit(0);
        }
        printHelp();
    }
    else if(strcmp(first,"prt")==0) 
    {   
        pid_t pid = fork();
        if (pid == 0) prt();
        else 
        {
            int status;
            wait(&status);
            exit(0);
        }
    }


    //Check all cases with multiple args
   

    //Checks for the 3 cd cases
    else if(strcmp(first,"cd") == 0)
    {
        second = strtok(NULL, " ");
        if(second == NULL)
        {
            prevDirectory = getcwd(prevBuffer, sizeof prevBuffer);
            chdir(getenv("HOME"));
            printf("%s\n", getenv("HOME"));
            prtValue = 0; //SUCCESS
            return;
        }

        else if(strcmp(second, "-") == 0)
        {
            char* holdDirectory;
            printf("%s\n", prevDirectory);
            holdDirectory = getcwd(buffer, sizeof buffer);
            chdir(prevDirectory);
            strcpy(prevDirectory, holdDirectory);
            prtValue = 0; //SUCCESS
            return;
        }
        else if (second != NULL) //User typed cd and at least a second arg.
        {
            DIR* dir = opendir(second);

            if (dir) //The directory is there so we can switch to it.
            {

                closedir(dir);
                prevDirectory = getcwd(prevBuffer, sizeof prevBuffer);
                chdir(second);
                printf("%s\n", prevDirectory);
                prtValue = 0; //SUCCESS
                return;
            }
            else     //The directory doesn't exist.
            {
                errno = ENOENT;
                printf("%s\n", "No such file or directory");
                prtValue = 1;
                return;
            }
        }
        else //The only input was cd.
        {
            prevDirectory = getcwd(prevBuffer, sizeof prevBuffer);
            chdir(getenv("HOME"));
            printf("%s\n", prevDirectory);
            prtValue = 0; //SUCCESS
            return;
        }
    }
    //Checks chpmt cases
    else if(strcmp(first,"chpmt") == 0)
    {
        second = strtok(NULL, " ");
        if(second == NULL) 
        {
            error();
            return;
        }
        third = strtok(NULL, " ");
        if(third == NULL)
        {
            error();
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
            error();
            return;
        } 
        third = strtok(NULL, " "); 
        if(third == NULL)
        {
            error();
            return;
        }
        fourth = strtok(NULL, " ");

        if(fourth == NULL)
        {
            error();
            return;
        }

        setColor(second, third, fourth);
        return;
    }
    else if(first != NULL)
    {
       // printf("%s\n", "Command not found");
       // prtValue = 127;
       // errno = EINVAL;
       // return; 
        executables(cmd);
    }

    
}