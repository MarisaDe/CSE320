#include "sfish.h"
#include <unistd.h>
#include <stdio.h>

char Ucolor[128];
char Mcolor[128];

 #define RED     "\033[22;31"
 #define GREEN   "\033[22;32"
 #define YELLOW  "\033[22;33"
 #define BLUE    "\033[22;34"
 #define MAGENTA "\033[22;35"
 #define CYAN    "\033[22;36"
 #define WHITE   "\033[22;37"
 #define BLACK   "\033[22;40"


void setColor(char* color, int colorFlag, int bold)
{
    char* user = getenv("USER");
    char machine[128];
    gethostname(machine, sizeof machine);


    //colorFlag. 1 = change user color, 2 = change machine color.
    if(colorFlag == 1)
    {
        if (strcmp(color,"red")==0) strcpy(Ucolor, RED);
        if (strcmp(color,"green")==0) strcpy(Ucolor, GREEN);
        if (strcmp(color,"yellow")==0) strcpy(Ucolor, YELLOW);
        if (strcmp(color,"blue")==0) strcpy(Ucolor, BLUE);
        if (strcmp(color,"magenta")==0) strcpy(Ucolor, MAGENTA);
        if (strcmp(color,"cyan")==0) strcpy(Ucolor, CYAN);  
        if (strcmp(color,"white")==0) strcpy(Ucolor, WHITE); 
        if (strcmp(color,"black")==0) strcpy(Ucolor, BLACK); 

        if(bold == 1) strcat(Ucolor, ";1m");
        else strcat(Ucolor, "m");
        strcat(Ucolor, user); 
        strcat(Ucolor, "\033[0m");
    }
    else if (colorFlag == 2)
    {
        if (strcmp(color,"red")==0) strcpy(Mcolor, RED);
        if (strcmp(color,"green")==0) strcpy(Mcolor, GREEN);
        if (strcmp(color,"yellow")==0) strcpy(Mcolor, YELLOW);
        if (strcmp(color,"blue")==0) strcpy(Mcolor, BLUE);
        if (strcmp(color,"magenta")==0) strcpy(Mcolor, MAGENTA);
        if (strcmp(color,"cyan")==0) strcpy(Mcolor, CYAN);  
        if (strcmp(color,"white")==0) strcpy(Mcolor, WHITE); 
        if (strcmp(color,"black")==0) strcpy(Mcolor, BLACK);   

        if(bold == 1) strcat(Mcolor, ";1m");
        else strcat(Mcolor, "m");
        strcat(Mcolor, machine);
        strcat(Mcolor, "\033[0m");   
    }
}


const char* sfish(int userFlag, int machineFlag, char* buffer)
{
    //char buffy[128];
    char cwd[128];
    //gethostname(buffy, sizeof buffy);
    getcwd(cwd, sizeof(cwd));
    //char user = getenv("USER");

    if(strcmp(cwd,getenv("HOME")) == 0)  strcpy(cwd,"~");
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


int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.


    //Initially set up the home variable
    chdir(getenv("HOME"));
    char *cmd;
    char *prevDirectory;
    prevDirectory = getenv("HOME");
    char buffer[1024];
    //char *sfishline = sfish(1,1);
    int userFlag = 1;
    int machineFlag = 1;
    //set initial colors to white
    setColor("white", 1, 1);
    setColor("white", 2, 0);

    while((cmd = readline((sfish(userFlag, machineFlag, buffer)))) != NULL) {
         //Exit or quit the program
        if (strcmp(cmd,"exit")==0)
            exit(3);

        printf("%s\n",cmd);

        if (strcmp(cmd,"help")==0)
        {
            printHelp();
        }
        //cd with no args: should go to the user’s home directory which is stored in the HOME env. variable
        if (strcmp(cmd,"cd")==0)
        {
            
            prevDirectory = getcwd(buffer, sizeof buffer);
            chdir(getenv("HOME"));
            printf("%s\n", getenv("HOME"));

        }

        //should change the working directory to the last directory the user was in.
        //Will work on this later, might need a stack of every directory change made.
        if(strcmp(cmd,"cd -")==0)
        {
            chdir(prevDirectory);
        }
        //if cmd contains cd and it's not just equal to cd, change working directory to what user specifies
        if(strstr(cmd, "cd ") != NULL && strcmp(cmd,"cd")!=0) 
        {
            char *space = " ";
            char *newDirectory;
            newDirectory = strtok(cmd, space);
            newDirectory = strtok(NULL, space);
            prevDirectory = getcwd(buffer, sizeof buffer);
            //printf("%s\n", newDirectory);
            chdir(newDirectory);
        }

        //pwd prints working directory
        if (strcmp(cmd,"pwd")==0) printf("%s\n", getcwd(buffer, sizeof buffer));

        //chmpt options
        //red
        if (strcmp(cmd,"chclr red user 1")==0) setColor("red", 1, 1);
        if (strcmp(cmd,"chclr red user 0")==0) setColor("red", 1, 0);
        if (strcmp(cmd,"chclr red machine 1")==0) setColor("red", 2, 1);
        if (strcmp(cmd,"chclr red machine 0")==0) setColor("red", 2, 0);




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
