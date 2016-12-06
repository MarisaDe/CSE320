#include "lott.h"
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


static void* map(void*);
static void* reduce(void*);
char** parseDir();
void* iterateMap(void*);

int part2(size_t nthreads) {


    ///////////////////////////////////////////////////////////////////////////
    //numfiles = nfiles(DATA_DIR);                          //checks to see how many files are in the dir so we know how many threads to spawn.
    numfiles = 10;
    int workload = 0;
    if(nthreads != 0) 
    {
        workload = 1 + ((numfiles - 1) / nthreads);         //Each thread will do workload files 
        printf("%s%i\n","Workload:", workload);
    }   
    else 
    {
        return -1;
    }                                  
    printf("%i\n",numfiles);                                //test the file path
    mapStruct* results = malloc(sizeof(mapStruct)*numfiles);
    memset(results, 0, sizeof(&results));
    // for(int i=0; i < nthreads; i++)
    // {
    //    results[i] = malloc(sizeof(mapStruct));
    //    memset(results[i], 0, sizeof(mapStruct));
    // }

    char ** allFiles = parseDir();                       //gathers all the files
    printf("%s",allFiles[0]);
    int start = 0;
    int end = workload-1;
    int range = end-start;
    pthread_t threadfile[nthreads];   //make a thread for each file
    mapStruct* result;
    part2Struct part2Array[numfiles];     //make a struct to store info about each file
    for(int i=0; i < nthreads; i++)                         //spawn the threads
    {
       //printf("%i%i\n",start, end);
       part2Array[i].start = start;
       //printf("%s%i\n","start: ", p->start);
       part2Array[i].end = end;
       part2Array[i].range = range;
       part2Array[i].allFiles = allFiles;
       //result = iterateMap(p);                          //need a pointer that points to an array
       pthread_create(&threadfile[i],NULL,iterateMap,&part2Array[i]); 
       //free(p);
       start+=workload;
       end+=workload;
       range = end-start;
       
     }
      int count = -1;
      for(int i=0; i < nthreads; i++)
      {
        pthread_join(threadfile[i], (void**)&result);
        printf("%s%f\n","avgDurInLoop: ", result[0].avgDur);
        for(int n=0; n < workload; n++)
        {
            count++;
            //memcpy(&results[count],result,sizeof(mapStruct));
            results[count] = result[n]; 
        }
        //printf("%s%f\n","avgDurInLoop: ", results[0].avgDur);
      }
    //allData->resultArray = results;
    reduceStruct* rResults; 

    //initialize the variables.
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
    for(int i = 0; i < numfiles; i ++)
    {
        free(allFiles[i]);
    }
    free(results); 
    free(allFiles);
    free(rResults);   
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

//Given the file name, it opens it and does an operation on it.
static void* map(void* v){
    mapStruct* f = (mapStruct*)v;
    FILE* fp;                                           //creates a file pointer
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
    f->ccodeToFree =(ccode);
    //////////////////////////////////////////////////////////////////////// 
    
    //PERFORM SOME ANALYSIS ON THIS
    if(current_query == A || current_query == B)
    {
        f->avgDur = avgDuration(fp, duration, i);
        printf("%s%f\n", "MAP: ", f->avgDur);
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
    //printf("%s%f", "REDUCE AVG: ", f[0].avgDur);
    //malloc(sizeof(mapStruct)*numfiles);
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

    //Gets MAX of averages users
    else if(current_query == C)
    {
        partC(f, compile);
        return compile;
    }

    //Gets MIN of averages users
    else if(current_query == D)
    {
        partD(f, compile);
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

            direntry = readdir(directory); //read the next file in the path
        }
        closedir(directory); //close the directory stream
    }
    return fileList;
}
////////////////////////////////////////////////////////////////////////////////////////
void* iterateMap(void* p)
{                                 
    printf("%s\n","Thread Spawned!!!!!");
    part2Struct* info = (part2Struct*) p;
    int range = info->range;
    range++;
    //printf("%s%i\n", "RANGE: ", range);
    //printf("%s%i\n", "start: ", info->start);
    //printf("%s%i\n", "end: ", info->end);
    mapStruct** mapArray = (mapStruct**) malloc(sizeof(mapStruct*)*range);  //create an array for the map struct to RETURN.
    memset(mapArray,0,sizeof(mapStruct*)*range);


    mapStruct *ret = malloc(sizeof(mapStruct*)*range);
    if(!ret)
        return NULL;

    for (int i =0 ; i < range; i++)
    {
        mapArray[i] = (mapStruct*)malloc(sizeof(mapStruct));
        memset(mapArray[i],0,(sizeof(mapStruct)));
    }

    mapStruct* f = malloc(sizeof(mapStruct));
    void* ppp;
    memset(f,0,sizeof(mapStruct));
    int arrCount = 0;
    for(int i = info->start; i <= info->end; i++)
    {
        //printf("%s%i\n", "Count: ", i);
        strcpy(f->file,DATA_DIR);
        strcat(f->file,"/");

        if(i < numfiles)
        {   
            strcat(f->file, info->allFiles[i]);        
            strcpy(f->filename, info->allFiles[i]);
            f->range = info->range;
            //printf("%s\n", f->file);
            ppp = map(f);
            memcpy(&ret[arrCount],ppp,sizeof(mapStruct));
            //printf("%s%f\n", "iterateMap: ", ret[i].avgDur);
            arrCount++;
        }
    }
    //free(f);  
    return ret;
} 