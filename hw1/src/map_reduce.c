//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "../include/map_reduce.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

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
printhelp();
return EXIT_FAILURE*-1; //return -1

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

/**
* The map function goes through each file in a directory, performs some action on
* the file and then stores the result.
*
* @param  dir       The directory that was specified by the user.
* @param  results   The space where map can store the result for each file.
* @param  size      The size of struct containing result data for each file.
* @param  act       The action (function map will call) that map will perform on
*					each file. Its argument f is the file stream for the specific
*					file. act assumes the filestream is valid, hence, map should
*					make sure of it. Its argument res is the space for it to store
*					the result for that particular file. Its argument fn is a
*					string describing the filename. On failure returns -1, on
*					sucess returns value specified in description for the act
*					function.
*@return 			The map function returns -1 on failure, sum of act results on 
*					success.
*/

int map(char* dir, void* results, size_t size, int (*act)(FILE* f, void* res, char* fn)){

int numfiles = 0;	//initializes number of files
int result = 0;		//keeps track of the return value of *act
int sum = 0;        //will coun all the results of every *act call.
numfiles = nfiles(dir); //checks to see how many files are in the dir
//continue if there is at least 1 file to map.
if(numfiles > 0)
{
	DIR *directory;			  //creates a directory pointer
	char filepath[100];
	directory = opendir(dir); //open the directory.

	if(directory) //if directory is even valid
	{
		struct dirent* direntry = readdir(directory);

		//Step 1: for each file in the directory:
		while(direntry != NULL)
		{	
			FILE * fp;                //creates a file pointer
			//int length = 0;		  //creates length for the string
			strcpy(filepath, dir);	  //copies the path

			if(dir[strlen(dir)-1] != '/') //if dir doesn't have / at the end
			{
				strcat(filepath, "/");//add the / for the whole path/.
			}


			if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0 )
			{	
				strcat(filepath, direntry->d_name); //Step 2: get full path of file
				//length = strlen(filepath);		//store length of string
				//printf("%s\n",filepath);			//test the file path

				fp = fopen(filepath, "r"); 			//Step 3: open file

				//Step 4: perform some action and store result.
				result = (*act)(fp,results,direntry->d_name);
				sum += result;
				memset(results,result,size);		//fills memory in results with the result	
				fclose(fp);							//Step 5: close file
			
			}
			direntry = readdir(directory); //read the next file in the path
		}

		closedir(directory); //close the directory stream
		return sum; //return sum of all elements in the result array.
	}

	else{
		return -1;
	}

}
else{
	return -1;
}
}

/**
* This reduce function takes the results produced by map and cumulates all
* the data to give one final Analysis struct. Final struct should contain
* filename of file which has longest line.
*
* @param  n        The number of files analyzed.
* @param  results  The results array that has been populated by map.
* @return          The struct containing all the cumulated data.
*/
struct Analysis analysis_reduce(int n,void* results)
{
	//Cast the void pointer as a struct Analysis pointer
	struct Analysis* iresults = (struct Analysis*) results;
	//Create the initial state of the struct Analysis to return
	struct Analysis anal_final;
	anal_final.lnno = 0;
	anal_final.lnlen = 0;
	anal_final.filename = NULL;
	memset(anal_final.ascii, 0, sizeof anal_final.ascii);


	//n = length of array of struct Analysis. 
	int i = 0;
	int ascii_index = 0;
	for(; i<n; i++)
	{
		//goes through every element in the ascii array of the struct and adds it to the anal_final
		for(; ascii_index<128; ascii_index++)
		{

			anal_final.ascii[ascii_index] += iresults[i].ascii[ascii_index];
		}

		ascii_index = 0;	//resets the index to reference ascii[ascii] to 0.

		if (iresults[i].lnlen > anal_final.lnlen)
		{
			anal_final.lnlen = iresults[i].lnlen;  //sets final struct Analysis lnlen to the struct with longest lnlen.
			anal_final.lnno = iresults[i].lnno;
			anal_final.filename = iresults[i].filename;
		}

	}
	return anal_final;

}

/**
 * This reduce function takes the results produced by map and cumulates all
 * the data to give one final Stats struct. Filename field in the final struct 
 * should be set to NULL.
 *
 * @param  n       The number of files analyzed.
 * @param  results The results array that has been populated by map.
 * @return         The struct containing all the cumulated data.
 */
Stats stats_reduce(int n, void* results)
{

	//Cast the void pointer as a struct Stats pointer
	struct Stats* iresults = (struct Stats*) results;
	printf("%d\n",iresults[n].n);

	//Create the initial state of the struct Stats to return
	Stats stats_final;
	stats_final.filename = NULL;
	stats_final.sum = 0;
	stats_final.n = 0;
	memset(stats_final.histogram, 0, sizeof stats_final.histogram);

	//create counter variables
	int i = 0;
	int histo_index = 0;
	for(; i<n; i++)
	{

		//goes through every element in the histogram array of the struct and adds it to the histo_final
		for(; histo_index<NVAL; histo_index++)
		{

			stats_final.histogram[histo_index] += iresults[i].histogram[histo_index];

		}

		histo_index = 0;	//resets the index to reference ascii[ascii] to 0.
	
		//update the sum and total count of nums in the files
		stats_final.sum+=iresults[n].sum;
		stats_final.n+=iresults[n].n;
		//printf("%d\n",iresults[n].n);


	}
	return stats_final;

}


/**
 * Always prints the following:
 * - The name of the file (for the final result the file with the longest line)
 * - The longest line in the directory's length.
 * - The longest line in the directory's line number.
 *
 * Prints only for the final result:
 * - The total number of bytes in the directory.
 *
 * If the hist parameter is non-zero print the histogram of ASCII character
 * occurrences. When printing out details for each file (i.e the -v option was
 * selected) you MUST NOT print the histogram. However, it MUST be printed for
 * the final result.
 *
 * Look at sample output for examples of how this should be print. You have to
 * match the sample output for full credit.
 *
 * @param res    The final result returned by analysis_reduce
 * @param nbytes The number of bytes in the directory.
 * @param hist   If this is non-zero, prints additional information. (Only non-
 *               zero for printing the final result.)
 */
void analysis_print(struct Analysis res, int nbytes, int hist)
{	
	//Print filename!
	printf("%s", "File: ");
	printf("%s\n", res.filename);

	//Print longest line length!
	printf("%s", "Longest line length: ");
	printf("%d\n", res.lnlen);

	//Print longest line in the directory's line number!
	printf("%s", "Longest line number: ");
	printf("%d\n", res.lnno);

	//Print bytes somewhere??? How to determine if res is the final struct idk.

	//Print histogram if it is non-zero. If nonzero, it prints final result I guess.

	int i;
	int p;
	if(hist !=0)
	{
		printf("%s", "Total Bytes in directory: ");
		printf("%d\n", nbytes);
		printf("%s\n", "Histogram:");

		for(i=0; i<128; i++)
		{
			//If there are elements in the index
			if(res.ascii[i] != 0)
			{
				//print out histogram of the index
				printf("%s%d%s", " ", i,":");

				//print the - for each instance
				for(p=0; p<res.ascii[i]; p++)
				{
					printf("%s", "-");
				}

				printf("%s","\n");
			}
			
		}
	}

}

/**
 * Always prints the following:
 * Count (total number of numbers read), Mean, Mode, Median, Q1, Q3, Min, Max
 *
 * Prints only for each Map result:
 * The file name
 *
 * If the hist parameter is non-zero print the the histogram. When printing out
 * details for each file (i.e the -v option was selected) you MUST NOT print the
 * histogram. However, it MUST be printed for the final result.
 *
 * Look at sample output for examples of how this should be print. You have to
 * match the sample output for full credit.
 *
 * @param res  The final result returned by stats_reduce
 * @param hist If this is non-zero, prints additional information. (Only non-
 *             zero for printing the final result.)
 */
void stats_print(Stats res, int hist)
{
	int mode, i, p, min,max, medianfound, pos1, pos2, indextot,pos1found, pos2found, q1pos, q3pos;
	int q1found, q3found, modefound;
	mode = 0;
	modefound = -1;
	q1found = -1;
	q3found = -1;
	min = -1;
	medianfound = -1;
	int isEven = 0;
	double mean, median, q1, q3; 


	//print filename if hist = 0
	if(hist == 0)
	{
		printf("%s%s\n","File: ",res.filename);
	}

	//print histogram if filename !=0
	else{

		printf("%s\n","Histogram:");
		for(i=0; i<NVAL; i++)
		{
			//If there are elements in the index
			if(res.histogram[i] != 0)
			{
				//print out histogram of the index
				printf("%d%s", i,"  :");

				//print the - for each instance
				for(p=0; p<res.histogram[i]; p++)
				{
					printf("%s", "-");
				}

				printf("%s","\n");
			}
			
		}
		printf("%s","\n");


	}
	
	printf("%s%d\n", "Count: ", res.n);
	//count is even		
	if(res.n %2 == 0)
	{	
		isEven = 1;
		pos1 = res.n/2;
		pos2 = res.n/2-1;
		pos1found = -1;
		pos2found = -1;
		indextot = 0;

	}
	//count is odd
	else{

		isEven = 0;
		median = res.n/2;
		indextot = 0;
	}

	q1pos = ceil(res.n *0.25);
	q3pos = ceil(res.n *0.75);

	for(i=0; i<NVAL; i++)
		{
			//If there are elements in the index
			if(res.histogram[i] > 0)
			{	
				if(min == -1) min = i; 	//set min to first instance of a number value	
				if(max < i) max = i;	//set max to last instance of a number value
				if(res.histogram[i] > modefound) modefound = res.histogram[i]; //set modefound to highest count of element

				indextot += res.histogram[i];

				//finds the q1
				if((q1pos == indextot || q1pos < indextot) && q1found != 1)
				{
					q1 = i;
					q1found = 1;
				}
				//finds the q3
				if((q3pos == indextot || q3pos < indextot) && q3found != 1)
				{
					q3 = i;
					q3found = 1;
				}


				//figures the median for an even count
				if(isEven && (pos1found == -1 || pos2found == -1))
				{
					if(pos1 == indextot || pos1 < indextot)
					{
						pos1= i;
						pos1found = 1;
					}
					if(pos2 == indextot || pos2 < indextot) 
					{
						pos2= i;
						pos2found = 1;
					}
				}
				//figures the median for an odd count
				else
				{
					if((median == indextot || median < indextot) && medianfound != 1)
					{
						median = i;	
						medianfound = 1;					
					}
				}
			}
			
		}

	if(isEven) median = (pos1 + pos2)/2;


	mean = (double)res.sum / (double)res.n;
	printf("%s%f\n", "Mean: ", mean);
	printf("%s", "Mode: ");

	//update the mode string
	for(i=0; i<NVAL; i++){

		if(modefound == res.histogram[i])
		{
			mode = i;
			printf("%i%s",mode, " ");
		}
	}
	
	printf("\n%s%f\n", "Median: ", median);
	printf("%s%f\n", "Q1: ", q1);
	printf("%s%f\n", "Q3: ", q3);
	printf("%s%d\n", "Min: ", min);
	printf("%s%d\n", "Max: ", max);


}


/**
 * This function performs various different analyses on a file. It
 * calculates the total number of bytes in the file, stores the longest line
 * length and the line number, and frequencies of ASCII characters in the file.
 *
 * @param  f        The filestream on which the action will be performed. You
 *                  can assume the filestream passed by map will be valid.
 * @param  res      The slot in the results array in which the data will be
 *                  stored.
 * @param  filename The filename of the file currently being processed.
 * @return          Return the number of bytes read.
 */
int analysis(FILE* f, void* res, char* filename){

	//Cast the void pointer as a struct Analysis pointer
	//res = (struct Analysis*) res;
	//Create the initial state of the struct Analysis to return
	//much of the code in cat can be used to calculate total number of bytes.
	char c;
	int n;
	while((c = fgetc(f)) != EOF) {
        n++;
    }

    rewind(f);
    int longestline = 0;
    int linenum = 0;
    int linenumcomp = 0;
    char line[1000];

    //goes through each line in the file
    	while (fgets(line, n, f) != NULL)
    	{
    		linenumcomp++;
    		//gets longest line of the line farthest down in the file
    		//gets longest line number
    		if(strlen(line) > longestline || strlen(line) == longestline) 
    			{
    				longestline = strlen(line);
    				linenum = linenumcomp;			
    			}
   			printf("Read Buffer: %s\n", line );

     	}

    printf("%s%d\n", "Longest line length: ", longestline);
    printf("%s%d\n", "Longest line num: ", linenum);
    return n;

}

