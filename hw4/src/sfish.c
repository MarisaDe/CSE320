#include "sfish.h"
#include <unistd.h>



const char* sfish(int userFlag, int machineFlag, char* buffer)
{
    char buffy[128];
    char cwd[128];
    gethostname(buffy, sizeof buffy);
    getcwd(cwd, sizeof(cwd));
    if(strcmp(cwd,getenv("HOME")) == 0)  strcpy(cwd,"~");

    if(userFlag == 1 && machineFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s%s%s", "sfish-", getenv("USER"), "@", buffy, ":", "[", cwd, "]> ");
        return buffer;
    }
    else if (userFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s", "sfish-", getenv("USER"), ":", "[", cwd, "]> ");
        return buffer;
    }

    else if(machineFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s", "sfish-", buffy, ":", "[", cwd, "]> ");
        return buffer;
    }

    else{

        snprintf(buffer, 1024, "%s%s%s%s", "sfish:", "[", cwd, "]> ");;
        return buffer;
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
    char *prevDirectory;
    prevDirectory = getenv("HOME");
    char buffer[1024];
    //char *sfishline = sfish(1,1);
    int userFlag = 1;
    int machineFlag = 1;

    while((cmd = readline((sfish(userFlag, machineFlag, buffer)))) != NULL) {
         //Exit or quit the program
        if (strcmp(cmd,"quit")==0 || strcmp(cmd,"exit")==0)
            exit(3);

        printf("%s\n",cmd);

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
            printf("%s\n", newDirectory);
            chdir(newDirectory);
        }

        //pwd prints working directory
        if (strcmp(cmd,"pwd")==0) printf("%s\n", getcwd(buffer, sizeof buffer));

        if (strcmp(cmd,"chmpt user 1")==0) userFlag = 1;

        if (strcmp(cmd,"chmpt user 0")==0) userFlag = 0;

        if (strcmp(cmd,"chmpt machine 1")==0) machineFlag = 1;

        if (strcmp(cmd,"chmpt machine 0")==0) machineFlag = 0;



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
