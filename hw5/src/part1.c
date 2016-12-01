#include "lott.h"
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>

static void* map(void*);
static void* reduce(void*);

int numfiles;

int part1(){


    /* DELETE THIS: YOU DO NOT CALL THSESE DIRECTLY YOU WILL SPAWN THEM AS THREADS */
    //map(NULL);
    reduce(NULL);
    /* DELETE THIS: THIS IS TO QUIET COMPILER ERRORS */


    ///////////////////////////////////////////////////////////////////////////

    numfiles = nfiles(DATA_DIR);      //checks to see how many files are in the dir so we know how many threads to spawn.
    printf("%i\n",numfiles);          //test the file path
    pthread_t threadfile[numfiles];   //make a thread for each file
    mapStruct mapArray[numfiles];     //make a struct to store info about each file
    int i = 0;
    if(numfiles > 0)
    {
        DIR *directory;                //creates a directory pointer
        char filepath[512];
        directory = opendir(DATA_DIR); //open the directory for data.

        if(directory)                  //if directory is valid
        {
            struct dirent* direntry = readdir(directory);
            while(direntry != NULL)     //while files are in the directory
            {   
                strcpy(filepath,DATA_DIR);
                strcat(filepath,"/");

                if(strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0 )
                {   
                    strcat(filepath, direntry->d_name);
                    //Step 4: perform some action and store result. 
                    strcpy(mapArray[i].file, filepath);
                    pthread_create(&threadfile[i],NULL,map, (void*) &mapArray[i]); 
                    i++;  
                }

                direntry = readdir(directory); //read the next file in the path
            }
            closedir(directory); //close the directory stream

            for(int n=0; n < numfiles; n++)
            {
                pthread_join(threadfile[n], NULL);
            }       
        }
    }
    /////////////////////////////////////////////////////////
    printf(
        "Part: %s\n"
        "Query: %s\n",
        PART_STRINGS[current_part], QUERY_STRINGS[current_query]);

    return 0;
}

//Given the file name, it opens it and does an operation on it.
static void* map(void* v){
    //printf("%s\n","IT CREATED A THREAD");
    mapStruct* f = (mapStruct*)v;
    FILE * fp;                                  //creates a file pointer
    char filepath[512];
    strcpy(filepath, f->file);
    fp = fopen(f->file, "r");   

    
    //PERFORM SOME ANALYSIS ON THIS
    if(current_query == A || current_query == B)
    {
        int duration[numfiles];                 //create an int array to store all the duration ints
        int n = 0;
        char *line = NULL;
        size_t len = 0;
        while(getline(&line, &len, fp) != -1)   //go through each line in the file
        {
            char* token = strtok(line, ",");
            for(int i = 0; i < 2; i++)
            {
                token = strtok(0, ",");
            }
            //printf("%s\n", token);
            duration[n] = atoi (token);
            printf("%i\n", duration[n]);
            n++;
        }

        float sum = 0;
        for(int i = 0; i < numfiles; i++)
        {
            printf("%i\n", duration[i]);
            sum += duration[i];
        }
        sum = sum/numfiles;
        printf("%f\n", sum);
        //fclose(fp);
        //return sum;
    }
    else if(current_query == C || current_query == D)
    {

    }
    else if(current_query == E)
    {

    }

    fclose(fp);                                 
    return NULL;
}

static void* reduce(void* v){
    return NULL;
}
