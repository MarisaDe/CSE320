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
    numfiles = nfiles(DATA_DIR);      //checks to see how many files are in the dir so we know how many threads to spawn.
    //numfiles = 10;
    printf("%i\n",numfiles);          //test the file path
    pthread_t threadfile[numfiles];   //make a thread for each file
    mapStruct mapArray[numfiles];     //make a struct to store info about each file
    mapStruct* results = malloc(sizeof(mapStruct)*numfiles);
    memset(results, 0, sizeof(&results));
    //mapResults* allData = malloc(sizeof(mapResults)*numfiles);
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
                //printf("%s%f\n","UserCount: ", results[n].userCount);
                //printf("%s%s\n","YEAR: ", result[n].year);
            }

            //allData->resultArray = results;
            reduceStruct* rResults;
            rResults = reduce(results);   

            printf(
            "Part: %s\nQuery: %s\nResult: %.5g, %s\n",
            PART_STRINGS[current_part], QUERY_STRINGS[current_query], rResults->result, rResults->filename);
            free(results);    
        }
    
    /////////////////////////////////////////////////////////
    // printf(
    //     "Part: %s\n"
    //     "Query: %s\n",
    //     PART_STRINGS[current_part], QUERY_STRINGS[current_query]);

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
    char* year;                              //array of all the years in the file
    char* ccode[lines];                             //array of all the country codes in the file
    char* ip[lines];                                //array of all the IP addresses in the file
    int i = 0;
    int* yearResults = malloc(sizeof(int)*lines);

    while(fgets(line, 512, fp) != NULL)             //go through each line in the file
    {      
        time_t rawtime;
        int transfer;
        struct tm * timeinfo;
        char buffer [512];
        char* time = strtok_r(line, ",", &saveptr); //parse for the first comma
        transfer = strtol(time, &ptr, 10);          //convert char to int
        rawtime = (long long)transfer;              //convert int to long int
        timeinfo = localtime(&rawtime);             //get the tm struct to use for strftime
        strftime (buffer,80,"%Y",timeinfo);         //Get the year! (19xx - 20xx)
        year = buffer;         
        //printf("%s\n", year);                     //gets the time (year) puts it in the array.
        yearResults[i] = strtol(year, &ptr, 10); 
        //printf("%i\n", yearResults[i]);

        ip[i] = strtok_r(0, ",", &saveptr);         //gets the IP address (useless)
        char* token = strtok_r(0, ",", &saveptr);   //gets the duration
        duration[i] = strtol(token, &ptr, 10);      //makes duration a double
        ccode[i]  = strtok_r(0, ",", &saveptr);     //gets country code.
        i++;
    }
    fclose(fp);
    free(line);
    //////////////////////////////////////////////////////////////////////// 
    
    //PERFORM SOME ANALYSIS ON THIS
    if(current_query == A || current_query == B)
    {
        f->avgDur = avgDuration(fp, duration, i);
        printf("%s%f\n", "Avg to return:", f->avgDur);
        return f;
    }
    else if(current_query == C || current_query == D)
    {
        f->userCount = countPerYear(fp, yearResults, i);
        //printf("%s%f\n", "Count to return:", f->userCount);
        return f;
    }
    else if(current_query == E)
    {
        printf("%s%i%s",ip[i],duration[i],ccode[i]);
        ccodes(fp);
    }                             
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////

static void* reduce(void* v){
    //printf("%s", "PLEASE DONT SEG FAULT DEAR GOD");
    mapStruct* f = (mapStruct*)v;
    malloc(sizeof(mapStruct)*numfiles);
    //int size = sizeof(mapStruct)/sizeof(f[0]);
    reduceStruct* compile = malloc(sizeof(reduceStruct));


    //Do operation based on query. This get MAX of avgs.
    if(current_query == A)
    {
          float max = 0;
          for(int i = 0; i < numfiles; i++)
          {
            printf("%s%f\n", "element: ", f[i].avgDur);

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
          //printf("%s%f\n", "MAX: ", max);
          return compile;
    }
    
    //Gets MIN of averages
    if(current_query == B)
    {
        float min = f[0].avgDur;
        for(int i = 0; i < numfiles; i++)
        {
            printf("%s%f\n", "element: ", f[i].avgDur);

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
        compile->maxDuration = min;
        compile->result = min;
        //printf("%s%f\n", "MIN: ", min);
        return compile;
     }

    if(current_query == C)
    {
        float maxUsers = f[0].userCount;
        for(int i = 0; i < numfiles; i++)
        {
            //printf("%s%f\n", "userCount: ", f[i].userCount);
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
        //compile->maxDuration = min;
        compile->maxUsers = maxUsers;
        compile->result = maxUsers;
        //printf("%s%f\n", "MIN: ", min);
        return compile;
    }

    if(current_query == D)
    {

        float minUsers = f[0].userCount;
        strcpy(compile->filename, f[0].filename);
        for(int i = 0; i < numfiles; i++)
        {
            //printf("%s%f\n", "userCount: ", f[i].userCount);
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
        //compile->maxDuration = min;
        compile->minUsers = minUsers;
        compile->result = minUsers;
        //printf("%s%f\n", "MIN: ", min);
        return compile;
    }

    
    return NULL;
}
