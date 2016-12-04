#ifndef LOTT2_H
#define LOTT2_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


extern int numfiles;
//This is the struct that the map function will return
typedef struct mapStruct{
    char file[512];             //this includes the data/ portion
    char filename[512];         //the filename after data/
    int numFiles;
    float avgDur;
    char* year;
    double avgUser;
    char* ccode;
    //double avgDuration;
    //double avgUser;
    //int countryCodes;

} mapStruct;

//This it the struct that the reduce function will return
typedef struct reduceStruct{
    char filename[512];
    int maxUsers;
    float minDuration;
    float maxDuration;
    char ccode;
    float result;

} reduceStruct;

typedef struct mapResults{
    mapStruct* resultArray; 
} mapResults;


//Functions

int nfiles(char* dir);

float avgDuration(FILE* fp,  int duration[], int size);

//int avgPerYear(FILE* fp);

int ccodes(FILE* fp);

#endif /* LOTT2_H */


