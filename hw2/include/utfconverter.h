#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>


#define MAX_BYTES 2
#define SURROGATE_SIZE 2
#define NON_SURROGATE_SIZE 2
#define NO_FD -1
#define OFFSET 4

#define FIRST  0
#define SECOND 1
#define THIRD  2
#define FOURTH 3

#ifdef __STDC__
#define P(x) x
#else
#define P(x) ()
#endif

/** The enum for endianness. */
typedef enum {LITTLE, BIG} endianness;

/** The struct for a codepoint glyph. */
typedef struct Glyph {
	unsigned char bytes[MAX_BYTES];
	endianness end;
	bool surrogate;
} Glyph;

/** The given filename. */
extern char* filename;

/** The usage statement. */
const char* USAGE[5] = { 
"Command line utility for converting UTF files to and from UTF-8, UTF-16LE and UTF-16BE.\n\n",
"Usage:  ./utf [-h|--help] -u OUT_ENC | --UTF=OUT_ENC IN_FILE [OUT_FILE]\n\n",
"    Option arguments:\n\t-h, --help\t\tDisplays this usage.\n\t-v, -vv\t\t\tToggles the verbosity of the program to level 1 or 2. \n\n",
"    Mandatory argument:\n\t-u OUT_ENC, --UTF=OUT_ENC\tSets the output encoding.\n\t\t\t\t\tValid values for OUT_ENC: 8, 16LE, 16BE\n\n"
"    Positional Arguments:\n\tIN_FILE\t\tThe file to convert.\n\t[OUT_FILE]\tOutput file name. If not present, defaults to stdout.\n",
};

/** Which endianness to convert to. */
extern endianness conversion;

/** Which endianness the source file is in. */
extern endianness source;

extern int verbose;

extern int numOfSurrogates;
extern int numOfAscii;
extern int numOfGlyphs;
/**
 * A function that swaps the endianness of the bytes of an encoding from
 * LE to BE and vice versa.
 *
 * @param glyph The pointer to the glyph struct to swap.
 * @return Returns a pointer to the glyph that has been swapped.
 */
Glyph* swap_endianness(Glyph* glyph);

/**
 * Fills in a glyph with the given data in data[2], with the given endianness 
 * by end.
 *
 * @param glyph 	The pointer to the glyph struct to fill in with bytes.
 * @param data[2]	The array of data to fill the glyph struct with.
 * @param end	   	The endianness enum of the glyph.
 * 
 * @return Returns a pointer to the filled-in glyph.
 */
Glyph* fill_glyph(Glyph* glyph, unsigned int data[2], endianness end);

/**
 * Writes the given glyph's contents to stdout.
 *
 * @param glyph The pointer to the glyph struct to write to stdout.
 */
void write_glyph(Glyph* glyph);

/**
 * Calls getopt() and parses arguments.
 *
 * @param argc The number of arguments.
 * @param argv The arguments as an array of string.
 */
void parse_args(int argc, char** argv);

/**
 * Prints the usage statement.
 */
void print_help();

/**
 * Prints info. for verbosity level 1.
 */
void verbose1();

/**
 * Prints info. for verbosity level 2.
 *
 * @param start The clock that starts the time.
 */
void verbose2(clock_t start);

/**
 * Closes file descriptors and frees list and possibly does other
 * bookkeeping before exiting.
 *
 * @param The fd int of the file the program has opened. Can be given
 * the macro value NO_FD (-1) to signify that we have no open file
 * to close.
 */
void quit_converter(int fd);
