#include "../include/map_reduce.h"

//Space to store the results for analysis map
struct Analysis analysis_space[NFILES];
//Space to store the results for stats map
Stats stats_space[NFILES];

//Sample Map function action: Print file contents to stdout and returns the number bytes in the file.
int cat(FILE* f, void* res, char* filename) {
    char c;
    int n = 0;
    printf("%s\n", filename);
    while((c = fgetc(f)) != EOF) {
        printf("%c", c);
        n++;
    }
    printf("\n");
    return n;
}

int main(int argc, char** argv) {
	int test = 0;
	//test = validateargs(argc, argv);
    //printf("%d\n", test); //test return value
    //int files = 0;
    //files = nfiles(argv[1]); //tests the directory based on input
    //printf("%i\n", files);   //tests the files counted from nfiles
    int mapp = 0;
    int (*act)(FILE*, void*, char*);
    act = cat;
    void* res;
    FILE* f;
    size_t size = 100;
    char* fn = "map_reduce.c";
    mapp = map(argv[1],analysis_space,size,act);


    return EXIT_SUCCESS;  
}
