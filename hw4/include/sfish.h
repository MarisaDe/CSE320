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