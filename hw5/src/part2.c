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
    char ** allFiles = parseDir();                       //gathers all the files
    printf("%s",allFiles[0]);
    int start = 0;
    int end = workload-1;
    //pthread_t threadfile[nthreads];   //make a thread for each file
    //void* result;
    for(int i=0; i < nthreads; i++)                         //spawn the threads
    {
           //printf("%i%i\n",start, end);
           part2Struct* p = malloc(sizeof(part2Struct));
           p->start = start;
           //printf("%s%i\n","start: ", p->start);
           p->end = end;
           p->allFiles = allFiles;
           iterateMap(p);
          // pthread_create(&threadfile[i],NULL,iterateMap,&p); 
          // pthread_join(threadfile[i], (void**)&result);
    //     //memcpy(&results[n],result,sizeof(mapStruct));
    //     //printf("%i%s%i\n", n, " UserCount: ", results[n].countryUsers);
    //     //printf("%i%s%s\n", n ," Code: ", results[n].ccode);
    //     //printf("%s%s\n","YEAR: ", result[n].year);
           start+=workload;
           end+=workload;
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


    //map(NULL);
    return 0;
}

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

///////////////////////////////////////////////////////////////////////////////////////
//Assumes the direntry is valid.
//A thread will call this function and this will loop the map function workload times.
// void* parseDir(void* f)
// {
//     printf("%s\n","I PARSED ");
//     part2Struct* t = (part2Struct*) f;
//     mapStruct* input = malloc(sizeof(mapStruct));
//     //mapStruct* output = malloc(sizeof(mapStruct));
//     mapStruct* mapArray[t->workload];              //keeps a mapStruct array of all the files the thread does.
//     //printf("%s%i\n","AVGD: ", t->workload);
//     int i = 0;
//     //printf("%s%s\n","filename: ", t->directory->d_name);
//     char filepath[512];
//     while(t->direntry != NULL && i < t->workload)     //while files are in the directory
//     {
//         printf("%s%s\n","tdir: ",t->direntry->d_name);
//         printf("%s%i\n","i: ",i);

//         strcpy(filepath,DATA_DIR);
//         strcat(filepath,"/");

//         if(strcmp(t->direntry->d_name, ".") != 0 && strcmp(t->direntry->d_name, "..") != 0 )
//         {
//             strcpy(input->filename, t->direntry->d_name);
//             strcat(filepath, t->direntry->d_name);
//             strcpy(input->file, filepath); 
//             //printf("%s%s\n","filename: ", t->direntry->d_name);
//             //open the file to parse.
//             mapStruct* output = map(input);                  //map returns a mapstruct (void*)!
//             //memcpy(&output,(map((void*)input)),sizeof(mapStruct));
//             mapArray[i] = output;                           //Put this mapStruct into a mapStruct array!
//             printf("%s%f\n","AVGD: ", mapArray[i]->avgDur);  
//             i++;
//         }
        
//          printf("%s%i\n","dfdddfi: ",i);
//          t->direntry = readdir(t->directory);            
//          //printf("%s%i\n","dfdddfi: ",i);
//     }
//     printf("%s\n","end");
//     free(input);
//     mapStruct** arrayPointer = mapArray;
//     return arrayPointer;

// }
////////////////////////////////////////////////////////////////////////////////////////
char** parseDir()
{
    //printf("%s\n","I PARSED ");
    DIR *directory;                //creates a directory pointer
    char filepath[512];
    directory = opendir(DATA_DIR); //open the directory for data.
    char** fileList = malloc(sizeof(char*)*numfiles);
    for (int i =0 ; i < numfiles; ++i)
    {
        fileList[i] = malloc(512 * sizeof(char));
    }

    //char* listptr;
    int i = 0;

    if(directory)                  //if directory is valid
    {
        struct dirent* direntry = readdir(directory);
        while(direntry != NULL && i < numfiles)     //while files are in the directory
        {   
            strcpy(filepath,DATA_DIR);
            strcat(filepath,"/");

            if(strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0 )
            {   
               
                strcat(filepath, direntry->d_name);
                //strcpy(mapArray[i].file, filepath);         //get the file name so map know which file to open (no data)
                fileList[i] = malloc(strlen(filepath));
                strcpy(fileList[i], filepath);              //Fill the list with all the files in the directory
                //printf("%s\n",fileList[i]);
                //pthread_create(&threadfile[i],NULL,map,&mapArray[i]); 
                //printf("%s\n",filepath);
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
    printf("%s\n","I GOT HERE");
    part2Struct* info = (part2Struct*) p;
    mapStruct* f = malloc(sizeof(mapStruct));
    for(int i = info->start; i < info->end; i++)
    {
        //f->file = (info->allFiles[i]);
        printf("%s\n", info->allFiles[i]);
        map(f);
    }
    return NULL;
} 