#ifndef LOTT2_H
#define LOTT2_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>


extern int numfiles;
//This is the struct that the map function will return
typedef struct mapStruct{
    char file[512];             //this includes the data/ portion
    char filename[512];         //the filename after data/
    int numFiles;
    float avgDur;
    char* year;
    float* avgUser;
    float userCount;
    char* ccode;
    int countryUsers;
    char** ccodeToFree;
    int countFree;
    int ccodeToFreeCount;
    int arrayRange;
    int range;               //for part 2. (range is #of files for each thread)
    //double avgDuration;
    //double avgUser;
    //int countryCodes;

} mapStruct;

//This it the struct that the reduce function will return
typedef struct reduceStruct{
    char filename[512];
    float maxUsers;
    float minUsers;
    float minDuration;
    float maxDuration;
    char* ccode;
    float result;
    float freq;
    char** ccodeToFree;
    int ccodeToFreeCount;

} reduceStruct;

typedef struct mapResults{
    mapStruct* resultArray; 
} mapResults;


typedef struct part2Struct{
    int workload;
    int start;
    int end; 
    int range;
    char ** allFiles;

} part2Struct;


//Functions

int nfiles(char* dir);

float avgDuration(FILE* fp,  int duration[], int size);

float countPerYear(FILE *fp, int year[], int size);

void ccodes(FILE* fp, char* ccode[],int size, mapStruct* f);

void reduceCcodes(mapStruct* f, reduceStruct* compile);

void partA(mapStruct* f, reduceStruct* compile);

void partB(mapStruct* f, reduceStruct* compile);

void partC(mapStruct* f, reduceStruct* compile);

void partD(mapStruct* f, reduceStruct* compile);


#endif /* LOTT2_H */


