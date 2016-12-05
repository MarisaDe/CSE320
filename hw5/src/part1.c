#include "lott.h"
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

static void* map(void*);
static void* reduce(void*);

int numfiles;

int part1(){


    /* DELETE THIS: YOU DO NOT CALL THSESE DIRECTLY YOU WILL SPAWN THEM AS THREADS */
    //reduce(NULL);
    /* DELETE THIS: THIS IS TO QUIET COMPILER ERRORS */


    ///////////////////////////////////////////////////////////////////////////
    //numfiles = nfiles(DATA_DIR);      //checks to see how many files are in the dir so we know how many threads to spawn.
    numfiles = 5;
    printf("%i\n",numfiles);          //test the file path
    pthread_t threadfile[numfiles];   //make a thread for each file
    mapStruct mapArray[numfiles];     //make a struct to store info about each file
    mapStruct* results = malloc(sizeof(mapStruct)*numfiles);
    memset(results, 0, sizeof(&results));
    int i = 0;
    if(numfiles > 0)
    {
        DIR *directory;                //creates a directory pointer
        char filepath[512];
        directory = opendir(DATA_DIR); //open the directory for data.

        if(directory)                  //if directory is valid
        {
            struct dirent* direntry = readdir(directory);
            while(direntry != NULL && i < numfiles)     //while files are in the directory
            {   
                strcpy(filepath,DATA_DIR);
                strcat(filepath,"/");

                if(strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0 )
                {   
                    strcpy(mapArray[i].filename, direntry->d_name);
                    strcat(filepath, direntry->d_name);
                    strcpy(mapArray[i].file, filepath);         //get the file name so map know which file to open
                    mapArray[i].numFiles = numfiles; 
                    //printf("%s\n",filepath);
                    pthread_create(&threadfile[i],NULL,map,&mapArray[i]); 
                    //printf("%s\n",filepath);
                    i++;  
                }

                direntry = readdir(directory); //read the next file in the path
            }
            closedir(directory); //close the directory stream
            mapStruct* result;
            for(int n=0; n < numfiles; n++)
            {
                pthread_join(threadfile[n], (void**)&result);
                memcpy(&results[n],result,sizeof(mapStruct));
                printf("%i%s%i\n", n, " UserCount: ", results[n].countryUsers);
                printf("%i%s%s\n", n ," Code: ", results[n].ccode);
                //printf("%s%s\n","YEAR: ", result[n].year);
            }

            //allData->resultArray = results;
            reduceStruct* rResults;
            rResults = reduce(results);   

            if(current_query != E)
            {
                printf(
                "Part: %s\nQuery: %s\nResult: %.7g, %s\n",
                PART_STRINGS[current_part], QUERY_STRINGS[current_query], rResults->result, rResults->filename);
            }
            else
            {
                printf(
                "Part: %s\nQuery: %s\nResult: %.7g, %s\n",
                PART_STRINGS[current_part], QUERY_STRINGS[current_query], rResults->result, rResults->ccode);
            }
            free(results); 
            free(rResults);   
        }
    }
    return 0;
}

//Given the file name, it opens it and does an operation on it.
static void* map(void* v){
    mapStruct* f = (mapStruct*)v;
    FILE* fp;                                       //creates a file pointer
    fp = fopen(f->file, "r");

    //Parse through the file and store it in one struct for any query to access.

    int lines = 0;
    char* line;
    line = malloc(sizeof(char)*512);
    char *ptr;
    char *saveptr;

    //We must get number of lines in the file first to make the arrays.
    while(fgets(line, 512, fp) != NULL)        
    {
        lines++;
    }

    fclose(fp);
    fp = fopen(f->file, "r");
    int duration[lines];                            //array of all the durations in the file
    char* year;                                     //array of all the years in the file
    char* ccode[lines];                           //array of all the country codes in the file
    int i = 0;
    int* yearResults = malloc(sizeof(int)*lines);
    time_t rawtime;
    int transfer;
    struct tm * timeinfo;
    char buffer[512];

    while(fgets(line, 512, fp) != NULL)             //go through each line in the file
    {      
        char* time = strtok_r(line, ",", &saveptr); //parse for the first comma
        transfer = strtol(time, &ptr, 10);          //convert char to int
        rawtime = (long long)transfer;              //convert int to long int
        timeinfo = localtime(&rawtime);             //get the tm struct to use for strftime
        strftime (buffer,80,"%Y",timeinfo);         //Get the year! (19xx - 20xx)
        year = buffer;           
        yearResults[i] = strtol(year, &ptr, 10); 

        char* token = strtok_r(0, ",", &saveptr);   //gets the IP address (useless)
        token = strtok_r(0, ",", &saveptr);         //gets the duration
        duration[i] = strtol(token, &ptr, 10);      //makes duration a double
        token = strtok_r(0, ",", &saveptr);         //gets country code.
        ccode[i] = malloc(strlen(token) + 1);
        strcpy(ccode[i], token);
        i++;
    }
    fclose(fp);
    free(line);
    f->ccodeToFree = ccode;
    //////////////////////////////////////////////////////////////////////// 
    
    //PERFORM SOME ANALYSIS ON THIS
    if(current_query == A || current_query == B)
    {
        f->avgDur = avgDuration(fp, duration, i);
        f->userCount = 0.0;
        
    }
    else if(current_query == C || current_query == D)
    {
        f->userCount = countPerYear(fp, yearResults, i);
        f->avgDur = 0.0;
        
    }
    else if(current_query == E)
    {
        ccodes(fp, ccode, i, f);
        f->userCount = 0.0;
        f->avgDur = 0.0;  
        return f;     
    }    
    else{
        return NULL;
    }

    free(yearResults);
    for(int i = 0; i < lines; i++)
    {
        free(ccode[i]);
    }                         
    return f;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

static void* reduce(void* v){
    mapStruct* f = (mapStruct*)v;
    //malloc(sizeof(mapStruct)*numfiles);
    reduceStruct* compile = malloc(sizeof(reduceStruct));


    //Do operation based on query. This get MAX of avgs.
    if(current_query == A)
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
        return compile;
    }
    
    //Gets MIN of averages
    else if(current_query == B)
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
        return compile;
     }

    //Gets MAX of averages users
    else if(current_query == C)
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
        return compile;
    }

    //Gets MIN of averages users
    else if(current_query == D)
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
        return compile;
    } 
    else if(current_query == E)
    {
        strcpy(compile->filename, f[0].filename);
        reduceCcodes(f,compile);
        return compile;
    }     
    return NULL;
}
