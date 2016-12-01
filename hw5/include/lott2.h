#ifndef LOTT2_H
#define LOTT2_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//This is the struct that the map function will return
typedef struct mapStruct{
    char file[512];
    int numFiles;
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

extern int numfiles;

//Functions

int nfiles(char* dir);

#endif /* LOTT2_H */


