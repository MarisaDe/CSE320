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

float countPerYear(FILE *fp, int year[], int size)
{
	int years[51] = {0};
	int sum = 0;
	for(int i = 0; i < size; i++)
	{
		int yearInt = year[i];  
		yearInt -= 1970;						//This will give us the proper index. (from 0 - 49)
		years[yearInt] = years[yearInt] + 1;    //tally up the users for each year.
	}

	int count = 0;
	for(int i = 0; i < 51; i++)
	{
		if(years[i] != 0)
		{
			sum += years[i];					//Gather total amount of users in a file
			count++;							//Gather total years used in the file.
		}		
	}
	return ((float)sum/(float)count);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ccodes(FILE *fp, char* ccode[],int size, mapStruct* f)
{
	//MAX 10 Country codes at any times
	char* trackCodes[10] = {0};
	int trackFreq[10] = {0};
	//Go through all the lines in the file and parse through the country codes
	for(int i = 0; i < size; i++)
	{
	 	for(int n = 0; n < 10; n++) 						//go through the track array to see if ccode matches
	 	{
	 		if(trackCodes[n] == NULL)
	 		{
	 			trackCodes[n] = ccode[i];
	 			trackFreq[n] = trackFreq[n]+1;
	 			n = 9;
	 		}
	 		else if(strcmp(ccode[i],trackCodes[n]) == 0)
	 		{
	 			trackFreq[n] = trackFreq[n] + 1; 			//update frequency array.
	 			n=9;
	 		}
	 	}
		 
	}

	int maxFreq = 0;
	for(int i = 0; i < 10; i++)
	{
		if(maxFreq < trackFreq[i])
		{
			maxFreq = trackFreq[i];		//obtain the highest freq
			f->ccode = trackCodes[i]; 	//Obtain country code.

		}
		else if(maxFreq == trackFreq[i] && strcmp(trackCodes[i], f->ccode) < 0)
        {
			maxFreq = trackFreq[i];		//obtain the highest freq
			f->ccode = trackCodes[i]; 	//Obtain country code.

        }

	}
	f->countryUsers = maxFreq;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void reduceCcodes(mapStruct* f, reduceStruct* compile)
{
	//MAX 10 Country codes at any times
	char* trackCodes[10] = {0};
	int trackFreq[10] = {0};

	//Initial stuff to print
	compile->ccode = f[0].ccode;		//It doesn't really matter what this is.
	strcpy(compile->filename, f[0].filename);

	//Go through all the lines in the file and parse through the country codes
	for(int i = 0; i < numfiles; i++)
	{
	 	for(int n = 0; n < 10; n++) 						//go through the track array to see if ccode matches
	 	{
	 		if(trackCodes[n] == NULL)
	 		{
	 			trackCodes[n] = f[i].ccode;
	 			trackFreq[n] = trackFreq[n] + f[i].countryUsers;
	 			n = 9;
	 		}
	 		else if(strcmp(f[i].ccode,trackCodes[n]) == 0)
	 		{
	 			trackFreq[n] = trackFreq[n] + f[i].countryUsers; 			//update frequency array.
	 			n = 9;
	 		}
	 	}
		 
	}

	int maxFreq = 0;
	compile->ccode = trackCodes[0];
	for(int i = 0; i < 10; i++)
	{
		if(maxFreq < trackFreq[i])
		{
			maxFreq = trackFreq[i];				//obtain the highest freq
			compile->ccode = trackCodes[i]; 	//Obtain country code.
		}
		else if(maxFreq == trackFreq[i] && strcmp(trackCodes[i], compile->ccode) < 0)
        {
			maxFreq = trackFreq[i];		//obtain the highest freq
			compile->ccode = trackCodes[i]; 	//Obtain country code.
        }
	}
	compile->result = maxFreq;
	compile->freq = maxFreq;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void partA(mapStruct* f, reduceStruct* compile)
{
   	float max = 0.0;
    max = f[0].avgDur;
    strcpy(compile->filename, f[0].filename);
    for(int i = 0; i < numfiles; i++)
    {

        if(f[i].avgDur > max)
        {
            max = f[i].avgDur; 
            strcpy(compile->filename, f[i].filename);      
        }
        if(f[i].avgDur == max && strcmp(f[i].filename, compile->filename) < 0)
        {
            max = f[i].avgDur; 
            strcpy(compile->filename, f[i].filename);      
        }
    }
    compile->maxDuration = max;
    compile->result = max;
    //return compile;
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void partB(mapStruct* f, reduceStruct* compile)
{
  	float min = 0.0;
    min = f[0].avgDur;
    strcpy(compile->filename, f[0].filename);
    for(int i = 0; i < numfiles; i++)
    {
        if(f[i].avgDur < min)
        {
            min = f[i].avgDur;
            strcpy(compile->filename, f[i].filename);
        }
        if(f[i].avgDur == min && strcmp(f[i].filename, compile->filename) < 0)
        {
            min = f[i].avgDur;
            strcpy(compile->filename, f[i].filename);
        }
    }
    compile->minDuration = min;
    compile->result = min;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void partC(mapStruct* f, reduceStruct* compile)
{
    float maxUsers = 0.0;
    maxUsers = f[0].userCount;
    strcpy(compile->filename, f[0].filename);
    for(int i = 0; i < numfiles; i++)
    {
        if(f[i].userCount > maxUsers)
        {
            maxUsers = f[i].userCount;
            strcpy(compile->filename, f[i].filename);
        }
        if(f[i].userCount == maxUsers && strcmp(f[i].filename, compile->filename) < 0)
        {
            maxUsers = f[i].userCount;
            strcpy(compile->filename, f[i].filename);
        }
    }
    compile->maxUsers = maxUsers;
    compile->result = maxUsers;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void partD(mapStruct* f, reduceStruct* compile)
{
    float minUsers = 0.0;
    minUsers = f[0].userCount;
    strcpy(compile->filename, f[0].filename);
    for(int i = 0; i < numfiles; i++)
    {
        if(f[i].userCount < minUsers)
        {
            minUsers = f[i].userCount;
            strcpy(compile->filename, f[i].filename);
        }
        if(f[i].userCount == minUsers && strcmp(f[i].filename, compile->filename) < 0)
        {
            minUsers = f[i].userCount;
            strcpy(compile->filename, f[i].filename);
        }
    }
    compile->minUsers = minUsers;
    compile->result = minUsers;

}
