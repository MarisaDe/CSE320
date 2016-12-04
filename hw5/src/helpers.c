#include "lott.h"
#include <stdio.h>
#include <dirent.h>

int nfiles(char* dir){
	SUPPRESS_UNUSED(); 
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

//////////////////////////////////////////////////////////////////////////////////////////////////////

float avgDuration(FILE *fp,  int duration[], int size )
{
	float avg = 0.0;
    for(int i = 0; i < size; i++)
    {
    	avg += (float) duration[i];					 //get the avg of the duration
    }
    avg = avg/(float)size;							 //divide by total number of lines
    return avg;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// int avgPerYear(FILE *fp)
// {
//     int count[numfiles];                      //create an int array to store all the duration ints
//     int n = 0;
//     char *line = NULL;
//     size_t len = 0;
//     while(getline(&line, &len, fp) != -1)     //go through each line in the file
//     {
//         char* time = strtok(line, ",");
//         //do something with the time so we know the year


//         for(int i = 0; i < 2; i++)
//         {
//             char* token = strtok(0, ",");
//         }
//         //printf("%s\n", token);
//         count[n] = (token);
//         printf("%i\n", count[n]);
//         n++;
//     }
//     return 0;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
int ccodes(FILE *fp)
{
    char* codes[numfiles];                  //create an int array to store all the codes
    int n = 0;
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, fp) != -1)   //go through each line in the file
    {
        char* token = strtok(line, ",");
        //do something with the time so we know the year
        for(int i = 0; i < 3; i++)
        {
            token = strtok(0, ",");
        }
        //printf("%s\n", token);
        codes[n] = token;				//puts the code in the array
        printf("%s\n", codes[n]);
        n++;
    }
    return 0;
}