#ifndef SFISH_H
#define SFISH_H

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

extern char Ucolor[128];
extern char Mcolor[128];
extern char prevBuffer[1024];
extern int userFlag;
extern int machineFlag;
extern char* prevDirectory;
extern int prtValue;
extern char* writeTo;

//length of argv;
extern int argvLen;
//An array of the user input
extern char** argv;

extern int commandLen;


typedef int bool;
#define true 1
#define false 0

typedef struct job{
    char** com; 
    int PID;
    int jobNum;
    char* command;
    char* status;
} job;


typedef struct command {
    char** command; 
    char* input;
    char* output; 
    bool pipe; 
    char* next; 
    char* prev; 
} command;



//An array of commands
extern command** commandArr;

//extern command* insert;



/*  
*	Sets the color and boldness of the prompt specified by colorFlag.
*   @param color - the color you want to be changed to
*   @param colorFlag - an int that should only be 1 or 2 which identifies what to change the color of.
*   @param bold - an int that should only be 0 or 1 which bolds the colored text
*/
void setColor(char* setting, char* color,  char* bold);

/*  
*	Sets up the prompt
*   @param buffer - buffer to print to
*   @return const char* - the prompt which will be displayed in the shell.
*/
const char* sfish(char* buffer);

/*
* 	Helper function. Prints the help menu.
*/
void printHelp();

/*
* 	Helper function. Checks all the chmpt cases.
* 	@param setting - setting (either machine or user)
*	@param toggle - toggle (either 0 off or 1 on)
*/

void chmpt(char* setting, char* toggle);

/*
* 	Checks the user input from the shell.
* 	@param cmd -  current input
*/
void builtins(char* cmd);


/*
* 	Helper method that prints the error message.	
*/
void error();

/*
* 	Helper method that parses through user input and puts it into an array.
*/
void parse(char* cmd);


void executables(char* cmd);

#endif