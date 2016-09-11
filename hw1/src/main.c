#include "map_reduce.h"

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

    

    //initialize variables for later usage
    int (*act)(FILE*, void*, char*);    //create a function pointer 
    int mapp = 0;                       //initalize return value for map function call later.
    int numoffiles = 0;
    int i = 0;
    memset(analysis_space, 0, sizeof analysis_space);
    memset(stats_space, 0, sizeof stats_space);
    //struct Stats stats_cum;             //create a Stats struct with garbage to use later

    ///////////////////////////////////////

    
    int test = 0;                      //initialize variable to hold the validateargs return
    test = validateargs(argc, argv);   //check the user input
    //printf("%d",test);

    //-h flag was choosen
    if(test == 0)
    {
        printhelp();
        return EXIT_SUCCESS;
    }
    //invalid input
    else if(test == -1)
    {
        printhelp();
        return EXIT_FAILURE;
    }
    //ana  (just the final result)
    else if(test == 1)
    {
        struct Analysis anal_cum;           //create an Analysis struct with garbage to use later
        act = analysis;                     //set the pointer to point at the analysis function
        numoffiles = nfiles(argv[2]);
        mapp = map(argv[2],analysis_space,sizeof(struct Analysis),act); //test map function
        anal_cum = analysis_reduce(numoffiles,analysis_space);
        analysis_print(anal_cum, mapp, 1);  //prints final

    }
    //stats (JUST the final result)
    else if(test == 2)
    {   
        struct Stats stats_cum;
        act = stats;                              //set the pointer to point at the stats function
        numoffiles = nfiles(argv[2]);
        mapp = map(argv[2],stats_space,sizeof(struct Stats),act); //test map function
        stats_cum = stats_reduce(numoffiles,stats_space);
        stats_print(stats_cum, 1);  //prints final
    }
    //-v ana (ALL + final result)
    else if(test == 3)
    {   
        struct Analysis anal_cum;           //create an Analysis struct with garbage to use later
        act = analysis;                              //set the pointer to point at the analysis function
        numoffiles = nfiles(argv[3]);
        mapp = map(argv[3],analysis_space,sizeof(struct Analysis),act); //test map function
        anal_cum = analysis_reduce(numoffiles,analysis_space);
        
        for(i=0;i<numoffiles; i++)
        {
             //printf("%i", analysis_space[i].lnno);
              analysis_print(analysis_space[i], 0, 0);
        }
        analysis_print(anal_cum, mapp, 1);  //prints final
    }
    //-v stats (all + final result)
    else if(test == 4)
    {
        struct Stats stats_cum;
        act = stats;                              //set the pointer to point at the stats function
        numoffiles = nfiles(argv[3]);
        mapp = map(argv[3],stats_space,sizeof(struct Stats),act); //test map function
        stats_cum = stats_reduce(numoffiles,stats_space);

         for(i=0;i<numoffiles; i++)
         {
             printf("%i", analysis_space[i].lnno);
             stats_print(stats_space[i], 0);
         }

        stats_print(stats_cum, 1);  //prints final
    } 

    return EXIT_SUCCESS;  
}


