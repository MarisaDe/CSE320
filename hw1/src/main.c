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
	//int test = 0;
	//test = validateargs(argc, argv);
    //printf("%d\n", test); //test return value
    //int files = 0;
    //files = nfiles(argv[1]); //tests the directory based on input
    //printf("%i\n", files);   //tests the files counted from nfiles
    //int mapp = 0;
    //int (*act)(FILE*, void*, char*);    //create a function pointer
    //act = cat;    //set the pointer to point at the cat function
    //size_t size = 100;  //test size      
    //mapp = map(argv[1],analysis_space,size,act); //test map function
    //printf("%i\n",mapp);
    //int counter = 0;


    //test analysis_reduce
    struct Analysis pointy[3]; 

    memset(pointy[0].ascii, 0, sizeof pointy[0].ascii);
    memset(pointy[1].ascii, 0, sizeof pointy[1].ascii);
    memset(pointy[2].ascii, 0, sizeof pointy[2].ascii);

    pointy[0].lnlen = 2;
    pointy[0].lnno = 63;
    pointy[0].filename = "foiled";
    pointy[0].ascii[1] = 3;

    pointy[1].lnlen = 50;
    pointy[1].lnno = 34;
    pointy[1].filename = "foiled";
    pointy[1].ascii[1] = 3;
    pointy[1].ascii[8] = 6;


    pointy[2].lnlen = 100;
    pointy[2].lnno = 26;
    pointy[2].filename = "thisisthelongest";
    pointy[2].ascii[1] = 5;



    struct Analysis result;    

    result = analysis_reduce(3,pointy);  


    // printf("%d\n", result.lnlen); 
    // printf("%d\n", result.lnno);
    // printf("%s\n", result.filename);


    // int i=0;
    // for (; i<128; i++)
    // {
    //     printf("%i\n", result.ascii[i]);
    // }


    // TEST stats_reduce
    Stats pointy2[3]; 
    memset(pointy2[0].histogram, 0, sizeof pointy2[0].histogram);
    memset(pointy2[1].histogram, 0, sizeof pointy2[1].histogram);
    memset(pointy2[2].histogram, 0, sizeof pointy2[2].histogram);

    pointy2[0].n = 30;
    pointy2[1].n = 10;
    pointy2[2].n = 20;

    pointy2[0].sum = 100;
    pointy2[1].sum = 150;
    pointy2[2].sum = 50;

    pointy2[0].histogram[0] = 5;
    pointy2[0].histogram[1] = 5;
    pointy2[1].histogram[0] = 2;
    pointy2[2].histogram[0] = 1;

    //printf("%d\n", pointy2[0].n);
    //printf("%d\n", pointy2[1].n);
    //printf("%d\n", pointy2[2].n);

    //Stats cumulate;


    //cumulate = stats_reduce(3, pointy2);

    //printf("%d\n", cumulate.sum); 
    //printf("%d\n", cumulate.n);

    //int i = 0;
    //for (; i<NVAL; i++)
    //{
        //printf("%i\n", cumulate.histogram[i]);
    //}


    // TEST ANALYIS_PRINT
    analysis_print(result,100,1);
    analysis_print(result,50,0);


    //TEST STATS_PRINT
    Stats teststat;
    memset(teststat.histogram, 0, sizeof teststat.histogram);

    teststat.n = 48;
    teststat.sum = 87;
    teststat.histogram[1] = 19;
    teststat.histogram[2] = 19;
    teststat.histogram[3] = 10;

    //printf("%d", teststat.histogram[0]);



    stats_print(teststat,1);

    return EXIT_SUCCESS;  
}


