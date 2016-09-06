//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "../include/map_reduce.h"
#include <sys/types.h>
#include <dirent.h>

//Implement map_reduce.h functions here.


//Prints the help menu
void printhelp(){

printf("Usage:  ./mapreduce [h|v] FUNC DIR\n");
printf("\t%.100s\n\t\t%.100s\n\t\t%.100s\n\t%.100s\n\t", "FUNC    Which operation you would like to run on the data:", "ana - Analysis of various text files in a directory.","stats - Calculates stats on files which contain only numbers.", "DIR     The directory in which the files are located.");
printf("%.100s\n\t%.100s\n\t%.100s\n\t", "Options:", "-h      Prints this help menu.","-v      Prints the map function’s results, stating the file it’s from.");
}

/**
 * Validates the command line arguments passed in by the user.
 * @param  argc The number of arguments.
 * @param  argv The arguments.
 * @return      Returns -1 if arguments are invalid (refer to hw document).
 *              Returns 0 if -h optional flag is selected. Returns 1 if analysis
 *              is chosen. Returns 2 if stats is chosen. If the -v optional flag
 *              has been selected, validateargs returns 3 if analysis
 *              is chosen and 4 if stats is chosen.
 */

int validateargs(int argc, char** argv){

// If no args are given (argc = 0)
if(argc == 0)
{
	printhelp();
	return EXIT_FAILURE*-1; //return -1
}

//check for -h
else if(strcmp(argv[1], "-h") == 0){
	printhelp();
	return EXIT_SUCCESS; //return 0
}

//check for just ana
else if(strcmp(argv[1], "ana") == 0) return 1; 


//check for just stats
else if(strcmp(argv[1], "stats") == 0) return 2; 


//check for -v
else if(strcmp(argv[1], "-v") == 0){

	if(strcmp(argv[2], "ana") == 0) return 3;			//-v ana returns 3
	else if(strcmp(argv[2], "stats") == 0) return 4;    //-v stats returns 4
	else{ 
		printhelp();
		return EXIT_FAILURE*-1;
	}
	
}
else{
	printhelp();
	return EXIT_FAILURE*-1; //return -1
}
}
