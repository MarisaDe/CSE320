#include "sfish.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char Ucolor[128];
char Mcolor[128];
char prevBuffer[1024];
int userFlag = 1;
int machineFlag = 1;
char* prevDirectory;
int prtValue;
char* writeTo = NULL;
char** argv;
command** commandArr;
int argvLen;
int commandLen;
//command* insert;

 #define RED     "\033[22;31"
 #define GREEN   "\033[22;32"
 #define YELLOW  "\033[22;33"
 #define BLUE    "\033[22;34"
 #define MAGENTA "\033[22;35"
 #define CYAN    "\033[22;36"
 #define WHITE   "\033[22;37"
 #define BLACK   "\033[22;40"


void error()
{
    errno = EINVAL;
    perror("sfish");
    prtValue = 1;
}


const char* sfish(char* buffer)
{
    char cwd[1024];
    char cwd2[1024];
    char* temp;
    getcwd(cwd, sizeof(cwd));
    temp = strstr(cwd,getenv("HOME"));
    char temp2[1024] = "";
    //If the homepath is in cwd, put a ~
    //if the home substring has been found
    if(strstr(cwd,getenv("HOME"))) 
    {       
        //temp = strtok(cwd,getenv("HOME"));
        if(strlen(temp)!= strlen(getenv("HOME")))   //if home length and directory length are not the same
        {
            temp =  strstr(temp, getenv("USER"));   //get the user substring ex: mdepasquale
            strtok(temp, "/");                      //tokenize the /  (mdepasquale/Desktop) -> (mdepasquale)
            temp = strtok(NULL, "/");              //tokenize the / once more (mdepasquale/Desktop/) -> (Desktop)
            strcpy(temp2,temp);                    //Both should be desktop atm.
            temp = strtok(NULL, "/");
             while(temp)
             {
                 strcat(temp2,"/");
                 strcat(temp2, temp);
                 temp = strtok(NULL, "/");
                 //temp = NULL;
             }
            strcpy(cwd2,"~/");
            strcat(cwd2,temp2);
        }
        else  strcpy(cwd2,"~");
    }
    if(userFlag == 1 && machineFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s%s%s", "sfish-", Ucolor, "@", Mcolor, ":", "[", cwd2, "]> ");
        return buffer;
    }
    else if (userFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s", "sfish-", Ucolor, ":", "[", cwd2, "]> ");
        return buffer;
    }

    else if(machineFlag == 1){

        snprintf(buffer, 1024, "%s%s%s%s%s%s", "sfish-", Mcolor, ":", "[", cwd2, "]> ");
        return buffer;
    }

    else{

        snprintf(buffer, 1024, "%s%s%s%s", "sfish:", "[", cwd2, "]> ");
        return buffer;
    }
  
}

void executables(char* cmd)
{
    char* input;
    char* output;
    struct stat buffer;
    char* checkpath;
    int checkstat = -1;
    char* cmd2 = strdup(cmd);
    char* iterate;
    int count = 0;
    iterate = strtok(cmd2, " ");
    while(iterate!=NULL)
    {
        count++;
        iterate = strtok(NULL, " ");
    }

    char path[1024];
    strcpy(path,getenv("PATH"));
    char buff[1024];
    char* space = " ";
    char* argv[count];  
    free(cmd2);
    char* fillArray;
    fillArray = strtok(cmd, space);
    int i = 0;
    char* argv2[count];
    while(fillArray != NULL)
    {
        argv[i] = fillArray;
        fillArray = strtok(NULL, space);
        i++;
    }
    argv[i] = NULL;                 //ARGV IS FILLED WITH ALL THE INPUT!
    //i = 0;

    checkpath = strtok(path, ":");
    for(int i = 0; i < count; i++)
    {
        if(strcmp(argv[i], ">") == 0 && (i+1) < count)
        {          
            argv2[i+1] = NULL;
            output = argv[i+1];
            while(checkpath!= NULL) //It's not in the path, try the next one.
            {
                snprintf(buff, sizeof(buff), "%s%s%s", checkpath, "/", argv[0]);
                checkstat = stat(buff, &buffer);
                if(checkstat == 0) //the cmd was found!!
                {
                    pid_t pid = fork();
                    if (pid == 0) 
                    {
                        int writeToFile = open(output, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        dup2(writeToFile, 1);
                        close(writeToFile);
                        prtValue = 0;
                        execv(buff, argv2);
                        return;
                    }
                    else 
                    {
                        int status;
                        wait(&status);
                        prtValue = WEXITSTATUS(status); 
                        return;
                    }  
                 return;   
                }
                checkpath = strtok(NULL, ":");     
            }
        }
        else if(strcmp(argv[i], "<") == 0 && (i+1) < count)
        {
         
            argv2[i+1] = NULL;
            input = argv[i+1];;
            while(checkpath!= NULL) //It's not in the path, try the next one.
            {
                snprintf(buff, sizeof(buff), "%s%s%s", checkpath, "/", argv[0]);
                checkstat = stat(buff, &buffer);
                if(checkstat == 0) //the cmd was found!!
                {
                    pid_t pid = fork();
                    if (pid == 0) 
                    {
                        int writeToFile = open(input, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        dup2(writeToFile, 0);
                        close(writeToFile);
                        prtValue = 0;
                        execv(buff, argv2);
                        return;
                    }
                    else 
                    {
                        int status;
                        wait(&status);
                        prtValue = WEXITSTATUS(status); 
                        return;
                    }  
                 return;   
                }
                checkpath = strtok(NULL, ":");     
            }
        }
        argv2[i] = argv[i];
        //argv[i] = fillArray;
        //fillArray = strtok(NULL, space);
        
    }
    

    // CHECK IF THERE IS A SLASH IN FRONT does it for us already


    if(cmd[0]=='/' || (cmd[0]=='.' && cmd[1]=='/'))
    {
        checkstat = stat(cmd, &buffer); 
        if(checkstat == 0) //the cmd was found!!
        {
            pid_t pid = fork();
            if (pid == 0) 
            {
                execv(cmd, argv);
                prtValue = 0;

            }
            else 
            {
                int status;
                wait(&status);
                prtValue = WEXITSTATUS(status); 
            }
        return;
        }
        prtValue = 1;
        perror("sfish");
        return;  
    }

    strcpy(path,getenv("PATH"));
    checkpath = strtok(path, ":");
    while(checkpath!= NULL) //It's not in the path, try the next one.
    {
        snprintf(buff, sizeof(buff), "%s%s%s", checkpath, "/", argv[0]);
        checkstat = stat(buff, &buffer);  
        if(checkstat == 0) //the cmd was found!!
        {
            pid_t pid = fork();
            if (pid == 0) 
            {
                prtValue = 0;
                execv(buff, argv);
            }
            else 
            {
                int status;
                wait(&status);
                prtValue = WEXITSTATUS(status); 
            }  
         return;   
        }
        checkpath = strtok(NULL, ":");     
    }
    prtValue = 127;
    errno = EINVAL;
    printf("%s%s\n",argv[0], ": command not found.");
    return;
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
    setColor("machine", "white", "1");

    while((cmd = readline((sfish(buffer)))) != NULL) {

        builtins(cmd);
        free(cmd);

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