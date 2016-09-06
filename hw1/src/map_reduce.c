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
if(argc == 0 || argc == 1)
{
	printhelp();
	return EXIT_FAILURE*-1; //return -1
}

//index 0 MUST be ./mapreduce
else if(strcmp(argv[0], "./mapreduce") == 0) {

	//check for -h
	if(strcmp(argv[1], "-h") == 0){
		printhelp();
		return EXIT_SUCCESS; //return 0
	}

	//for these we must check if the DIR is valid.
	//first, check to see if a DIR arg was passed.
	//DIR can only be in argv index 2 if -v is not used.
	else if(argc == 3){

		//check to see if DIR is valid before checking FUNC
		DIR* dir = opendir(argv[2]);   //assume input DIR is in index 2.
		
		//if DIR is valid, close it and continue checking.
		if(dir)
		{
			closedir(dir);

			//check for just ana
			if(strcmp(argv[1], "ana") == 0) return 1; 

			//check for just stats
			else if(strcmp(argv[1], "stats") == 0) return 2;

			//if the FUNC is wrong, print help and return fail.
			else{
				printhelp();
				return EXIT_FAILURE*-1; //return -1
			}	 
		}

		//else if DIR is invalid, print help and return failure.
		else{
			printhelp();
			return EXIT_FAILURE*-1; //return -1
		}
	}

	//DIR can only be in argv index 3 if -v IS used.	
	else if(argc == 4){	

		//-v must be in index 1 in this case!
		if(strcmp(argv[1], "-v") == 0){
			//check to see if DIR is valid before checking FUNC
			DIR* dir2 = opendir(argv[3]);   //assume input DIR is in index 3.
		
			//if DIR is valid, close it and continue checking.
			if(dir2)
			{
				closedir(dir2);
				if(strcmp(argv[2], "ana" ) == 0) return 3;	//-v ana returns 3
				else if(strcmp(argv[2], "stats") == 0) return 4; //-v stats returns 4
				//if the FUNC is wrong, print help and return failure.
				else{ 
				printhelp();
				return EXIT_FAILURE*-1;
				}
			}
			//DIR is invalid. Print help menu and return failure.
			else{
				printhelp();
				return EXIT_FAILURE*-1; //return -1
			}	
		}	
	
	}
	else{
		printhelp();
		return EXIT_FAILURE*-1; //return -1
	}
}
//If it doesn't start with ./mapreduce it fails.
else{
	printhelp();
	return EXIT_FAILURE*-1; //return -1
	}
}



/**
* Counts the number of files in a directory EXCLUDING . and ..
* @param dir The directory for which number of files is desired.
* @return The number of files in the directory EXCLUDING . and ..
* If nfiles returns 0, then print "No files present in the
* directory." and the program should return EXIT_SUCCESS.
* Returns -1 if any sort of failure or error occurs.
*/
int nfiles(char* dir){


	//first check to see if the directory is valid.
	DIR *directory;
	directory = opendir(dir);
	int count = 0; 
	//directory is valid. Now check the files.
	if(directory)
	{
		struct dirent* direntry = readdir(directory);
		//while a file exists in the directory
		while(direntry != NULL)
		{
			//count all files that do not include "." and ".."
			//d_name is an array of the file name currently pointed at.
			if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0 )
			{
				count++;				    //count the file			
			}

			direntry = readdir(directory);	//read the next file in the stream
		}

		closedir(directory); 				//close the directory stream
		return count;		 				//return the number of files counted

	}
	//file is invalid. Returns -1.
	else{

		closedir(directory);
		return -1;
	}

}