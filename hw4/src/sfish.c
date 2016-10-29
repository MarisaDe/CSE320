#include "sfish.h"
#include <unistd.h>

int main(int argc, char** argv) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.

    char *cmd;
    while((cmd = readline("sfish> ")) != NULL) {

         //Exit or quit the program
        if (strcmp(cmd,"quit")==0 || strcmp(cmd,"exit")==0)
            exit(3);

        printf("%s\n",cmd);

        //cd no args
        if (strcmp(cmd,"cd")==0)
        {
            char buffer[1024];
            printf("%s\n", getcwd(buffer, sizeof buffer));

        }

        //pwd
        if (strcmp(cmd,"pwd")==0)
        {
            char buffer[1024];
            printf("%s\n", getcwd(buffer, sizeof buffer));

        }


        //All your debug print statments should be surrounded by this #ifdef
        //block. Use the debug target in the makefile to run with these enabled.
        #ifdef DEBUG
        fprintf(stderr, "Length of command entered: %ld\n", strlen(cmd));
        #endif
        //You WILL lose points if your shell prints out garbage values.

    }

    //Don't forget to free allocated memory, and close file descriptors.
    free(cmd);
    //WE WILL CHECK VALGRIND!

    return EXIT_SUCCESS;
}
