#include "lott.h"
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#define _GNU_SOURCE


static void* map(void*);
static void* reduce(void*);
char** parseDir();
void* iterateMap(void*);

int part2(size_t nthreads) {


    ///////////////////////////////////////////////////////////////////////////
    //numfiles = nfiles(DATA_DIR);                          //checks to see how many files are in the dir so we know how many threads to spawn.
    numfiles = 5;
    int workload = 0;
    if(nthreads != 0 && nthreads <= numfiles) 
    {
        workload = 1 + ((numfiles - 1) / nthreads);         //Each thread will do workload files 
    } 
    else if(nthreads > numfiles)                            //If user is idiot and has more threads than files, do as if 1 thread per file.
    {
        nthreads = numfiles;
        workload = 1;
    }  
    else 
    {
        return -1;
    }                                  

    //declare variables
    mapStruct* results = malloc(sizeof(mapStruct)*numfiles);
    memset(results, 0, sizeof(&results));
    char ** allFiles = parseDir();                          //Gathers all the files
    //printf("%s",allFiles[0]);   
    int start = 0;
    int end = workload-1;
    int range = end-start;
    pthread_t threadfile[nthreads];                         //make a thread for each file
    mapStruct* result;
    part2Struct part2Array[nthreads];
    char thread_name[16];
    //spawns n number of threads 
    for(int i=0; i < nthreads; i++)                         
    {
        part2Array[i].start = start;
        part2Array[i].end = end;
        part2Array[i].range = range;
        part2Array[i].allFiles = allFiles;
        pthread_create(&threadfile[i],NULL,iterateMap,&part2Array[i]); 
        char buffer[512];
        sprintf(buffer, "map %d", i);
        pthread_setname_np(threadfile[i], buffer); //Name ze thread
        pthread_getname_np(threadfile[i], thread_name, sizeof(thread_name));
        start+=workload;
        end+=workload;
        range = end-start;
    }

    //join the threads
    int count = -1;
    mapStruct* freeResults[nthreads];

    for(int i=0; i < nthreads; i++)
    {
        pthread_join(threadfile[i], (void**)&result);
        for(int n=0; n < result[0].range; n++)
        {
            count++;
            results[count] = result[n]; 
        }
        freeResults[i] = result;
    }
    reduceStruct* rResults; 
    rResults = reduce(results);   

    //print results
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
    for(int i = 0; i < numfiles; i ++)
    {
        free(allFiles[i]);
    }
    for(int i = 0; i < nthreads; i ++)
    {
        free(freeResults[i]);
    }  
    free(results); 
    free(allFiles);
    free(rResults);   
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//Given the file name, it opens it and does an operation on it
////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    char* ccode[lines];                             //array of all the country codes in the file
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
        token[2] = 0;
        ccode[i] = malloc(strlen(token)+1);         //store the country code in the index
        strcpy(ccode[i], token);
        i++;
    }
    fclose(fp);
    free(line);
    f->ccodeToFreeCount = i;
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
        f->ccodeToFree = ccode;
        f->ccodeToFreeCount = i;
        ccodes(fp, f->ccodeToFree,i, f);
        f->userCount = 0.0;
        f->avgDur = 0.0;       
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
// reduce function should only be called 1 time
/////////////////////////////////////////////////////////////////////////////////////////////////////
static void* reduce(void* v){
    mapStruct* f = (mapStruct*)v;
    reduceStruct* compile = malloc(sizeof(reduceStruct));
    //Do operation based on query. This get MAX of avgs.
    if(current_query == A)
    {
        partA(f, compile);
        return compile;
    }
    
    //Gets MIN of averages
    else if(current_query == B)
    {
        partB(f, compile);
        return compile;
     }

    //Gets MAX of users
    else if(current_query == C)
    {
        partC(f,compile);
        return compile;
    }

    //Gets MIN of users
    else if(current_query == D)
    {
        partD(f,compile);
        return compile;
    } 
    //Max freq of users from a ccode
    else if(current_query == E)
    {
        strcpy(compile->filename, f[0].filename);
        reduceCcodes(f,compile);
        return compile;
    }     
    return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////
//Parses all the files in the directory and returns a char pointer to it
////////////////////////////////////////////////////////////////////////////////////////
char** parseDir()
{
    //printf("%s\n","I PARSED ");
    DIR *directory;                                 //creates a directory pointer
    directory = opendir(DATA_DIR);                  //open the directory for data.
    char** fileList = (char**)malloc(sizeof(char*)*512);
    memset(fileList,0,(sizeof(char*)*512));
    int i = 0;

    if(directory)                                   //if directory is valid
    {
        struct dirent* direntry = readdir(directory);
        while(direntry != NULL && i < numfiles)     //while files are in the directory
        {   
            if(strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0 )
            {   
                fileList[i] = malloc(strlen(direntry->d_name)+1);
                strcpy(fileList[i], direntry->d_name);                //Fill the list with all the files in the directory
                i++;  
            }

            direntry = readdir(directory);          //read the next file in the path
        }
        closedir(directory);                        //close the directory stream
    }
    return fileList;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//Each thread calls this to go through the map function "workload" times. (the range from end-start)
//////////////////////////////////////////////////////////////////////////////////////////////
void* iterateMap(void* p)
{                                 
    part2Struct* info = (part2Struct*) p;
    //int range = info->range;
    //range++;
    int range = 0;
    for(int i = info->start; i <= info->end; i++)                   //for the range given, analyze the file using map
    {
        if(i < numfiles)                                            //only enter map if the index is less than # of files
        {
            range++;
        }
    }

    mapStruct* ret = malloc(sizeof(mapStruct)*range);               //create an array of the mapStructs to return
    for (int i =0 ; i < range; i++)
    {   
        memset(&ret[i],0,(sizeof(mapStruct)));
    }

    mapStruct* f = malloc(sizeof(mapStruct));
    memset(f,0,sizeof(mapStruct));
    int arrCount = 0;
    for(int i = info->start; i <= info->end; i++)                   //for the range give, analyze the file using map
    {
        strcpy(f->file,DATA_DIR);                                   //append data/ for the filename to open later.
        strcat(f->file,"/");

        if(i < numfiles)                                            //only enter map if the index is less than # of files
        {   
            strcat(f->file, info->allFiles[i]);        
            strcpy(f->filename, info->allFiles[i]);
            f->range = arrCount;
            map(f);
            memcpy(&ret[arrCount],f,sizeof(mapStruct));
            arrCount++;
        }
    }
    ret->range = arrCount;  //keep the count so we can move the info properly later
    free(f);                //free up space that's not needed anymore
    return ret;             //return an array of mapStructs
} 

