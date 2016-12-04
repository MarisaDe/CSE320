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
    char file[512];
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
    int maxUsers;
    double minDuration;
    double maxDuration;
    int countryCodes;

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


