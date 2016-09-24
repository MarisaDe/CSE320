#include "utfconverter.h"

char* filename;
endianness source;
endianness conversion;
int verbose;
int numOfGlyphs = 0;
int numOfSurrogates = 0;
int numOfAscii = 0;

 Glyph* swap_endianness(Glyph* glyph)
 {
 	/*Surrogates do the same thing as non-surrogates.*/
 	unsigned int swap = glyph->bytes[0];
 	glyph->bytes[0] = glyph->bytes[1];
 	glyph->bytes[1] = swap;
 	glyph->end = conversion;
 	return glyph;
 }



Glyph* fill_glyph(Glyph* glyph,unsigned int data[2],endianness end) 
{
 	glyph->bytes[0] = data[0];
 	glyph->bytes[1] = data[1];
 	printf("%s%x\n", "first data: ",data[0]);
 	printf("%s%x\n", "second data :",data[1]);

 	unsigned int bits = 0; 
 	/*Creates the code point*/
 	/*These would be non-surragate pairs.*/
 	if(end == BIG)
 	{
 		bits |= (data[0] + (data[1] << 8));
 		glyph->bytes[0] = data[1];
 		glyph->bytes[1] = data[0];
 		printf("%s%x\n", "combined w/ BE: ",bits);

 	}
 	else if(end == LITTLE)
 	{
 		bits |= ((data[0]<<8) + data[1]);
 		printf("%s%x\n", "combined w/ LE: ",bits);

 	}


 	/*Check for surrogate pairs*/
 	if(bits > 0x10000000 || bits == 0x10000000)
 	{	
 		glyph->surrogate = true;
 		numOfSurrogates++;
 		unsigned int high, low = 0;
 		bits = bits - 0x10000000;
 		high = (bits >> 10);
 		high+= 0xD8000000;
 		low =(bits ^= 0x3FF00000);
 		low+= 0xDC000000;
 		printf("%s%x\n", "surrogate high:", high);
 		printf("%s%x\n", "surrogate low:", low);
 		glyph->bytes[0] = data[high];
 		glyph->bytes[1] = data[low];

 	}
 	else
 		glyph->surrogate = false;
 		numOfAscii++;

  	glyph->end = end;
  	return glyph;
 }

void write_glyph(Glyph* glyph)
 {
 	if(glyph->surrogate){
 		write(STDOUT_FILENO, glyph->bytes, SURROGATE_SIZE);
 	} else {
 		write(STDOUT_FILENO, glyph->bytes, NON_SURROGATE_SIZE);
 	}
 }

 void parse_args(int argc, char** argv)
 {
 	verbose = 0;
 	int c;
 	int option_index = 0;
 	char* endian_convert;

 	struct option long_options[] = 
	{
		{"help", no_argument, 0, 'h'},
		{"UTF", required_argument, 0, 'u'},
		{"v", no_argument, 0, 'v'},
		{0, 0, 0, 0},

	};

	/*If getopt() returns with a valid (its working correctly)  return code, then process the args! */
 	while((c = getopt_long(argc, argv, "hvu:", long_options, &option_index)) != -1)
 	{
 		switch(c){ 
 			case 'h':
 				print_help();
 				quit_converter(NO_FD);
 				break;
 			case 'v':
 			    verbose++;
 				break;

 			case 'u':
 				endian_convert = optarg;
 				if(endian_convert == NULL)
 				{
 					fprintf(stderr, "OUT_ENC not given.\n");
 					print_help();
 				}

 				else if(strcmp(endian_convert, "16LE") == 0)
 				{ 
 					conversion = LITTLE;
 				} 
 				else if(strcmp(endian_convert, "16BE") == 0)
 				{
 					conversion = BIG;
 				} 
 				else
 				{		
 					fprintf(stderr, "Invalid OUT_ENC argument.\n");
 					print_help();
 				}
 				if(optind < argc) 
 				{
 					strcpy(filename, argv[optind]);
 				}
 				else 
 				{	
 					fprintf(stderr, "Filename not given.\n");
 					free(filename);
 					print_help();
 					quit_converter(NO_FD); 
 				}

 				break;
 			default:
 				fprintf(stderr, "Unrecognized argument.\n");
 				quit_converter(NO_FD);
 				break;
 		}

 	}
if(verbose > 3) verbose = 2;
return;
}


void verbose1()
{
	struct utsname info;
	uname(&info);
	struct stat *buf;
	buf = malloc(sizeof(struct stat));
	stat(filename, buf);
	double kb = 0.0;
	kb = ((double)(buf->st_size)) /1000;

	fprintf(stderr,"\t%s%f%s\n","Input file size: ",kb, " kb");
	fprintf(stderr,"\t%s","Input file path: \n");
	if(source == LITTLE) 	fprintf(stderr,"\t%s","Input file encoding: UTF-16LE\n");
	else if(source == BIG) 	fprintf(stderr,"\t%s","Input file encoding: UTF-16BE\n");
	if(conversion == LITTLE) 	fprintf(stderr,"\t%s","Output encoding: UTF-16LE\n");
	else if(conversion == BIG)	fprintf(stderr,"\t%s","Output encoding: UTF-16BE\n");

	fprintf(stderr,"\t%s%s\n","Hostmachine: ", info.machine);
	fprintf(stderr,"\t%s","Operating System: ");
	fprintf(stderr,"%s\n",info.sysname);
	free(buf);
}

void verbose2(clock_t start)
{	
	clock_t end = clock();
	double cpu_time_used = ((double) (end - start)/CLOCKS_PER_SEC);
	verbose1();
	fprintf(stderr,"\t%s%f\n","Reading: real=",cpu_time_used);
	fprintf(stderr,"\t%s%i%s\n","ASCII: ", ((numOfAscii-1)/numOfGlyphs)*100, "%");
	fprintf(stderr,"\t%s%i%s\n","Surrogates: ",(numOfSurrogates/numOfGlyphs)*100, "%");
	fprintf(stderr,"\t%s%i\n","Glyphs: ",numOfGlyphs);


	//Reading: real=2.4, user=1.1, sys=.6
//Converting: real=1.1, user=.4, sys=.1
//Writing: real=3.5, user=2.1, sys=1.2


}
void print_help() {
	int i;
	for(i = 0; i < 5; i++){
		printf("%s", USAGE[i]); 
	}
	quit_converter(NO_FD);
}

void quit_converter(int fd)
{
	//free(filename);
	close(STDERR_FILENO);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	if(fd != NO_FD)
		close(fd);
	exit(0);
	/*Ensure that the file is included regardless of where we start compiling from.*/
}

int main(int argc, char** argv)
{
	/*Start the clock*/
	clock_t start = clock();
	/*After calling parse_args(), filename and conversion should be set.*/
	filename = calloc(strlen(argv[argc-1])+1,sizeof(char));
	parse_args(argc, argv);
	/*Check to see if valid filename has a / in front. MUST REMOVE OTHERWISE WONT WORK!*/
	/*if(filename[0] == '/')
	{
		Increase pointer to next char to remove the 
		filename++; 
	}*/


	int fd = open(filename, O_RDONLY); 
	/*rv is for read*/
	int rv = 0;

	if(fd < 0)
	{
		fprintf(stderr, "Not a valid filename.\n"); 
 		free(filename);
		print_help();
	}

	/*Print the info for verbosity level 1.*/
	if(verbose ==1) verbose1();

	unsigned int buf[2] = {0,0};

	Glyph* glyph = malloc(sizeof(Glyph)); 

	/*Handle BOM bytes for UTF16 specially.*/
    /* Read our values into the first and second elements.*/
	if((rv = read(fd, &buf[0], 1)) == 1 &&  (rv = read(fd, &buf[1], 1)) == 1)
	{ 
		if(buf[0]== 0xff && buf[1] == 0xfe)
		{
			/*file is little endian FFFE*/
			source = LITTLE; 
 		} 
		else if(buf[0] == 0xfe000000 && buf[1] == 0xff000000)
		{
 			/*file is big endian FEFF*/
 			source = BIG;
 		} 
			
		else {
 			/*file has no BOM*/
 			free(&glyph->bytes); 
 			free(filename);
 			fprintf(stderr, "File has no BOM.\n");
 			print_help();
 			quit_converter(NO_FD); 
 		}
 		void* memset_return = memset(glyph, 0, sizeof(Glyph));
 		/*Memory write failed, recover from it:*/
 		if(memset_return == NULL)
 		{
 			/*tweak write permission on heap memory.
 			asm("movl $8, %esi\n\t"
 			    "movl $.LC0, %edi\n\t"
 			    "movl $0, %eax");
 			Now make the request again.*/
 			memset(glyph, 0, sizeof(Glyph));

 		}
	}

	/*Accounts for endianess and swaps it*/
	if(conversion != source)
	{
		glyph = fill_glyph(glyph, buf, source);
		write_glyph(swap_endianness(glyph));
	}

	/*Now deal with the rest of the bytes.*/
 	while((rv = read(fd, &buf[0], 1)) == 1 &&  (rv = read(fd, &buf[1], 1)) == 1)
	{	
		if(conversion == source)
		{
 			write_glyph(fill_glyph(glyph, buf, source));
 		}
 		else
 		{
 			glyph = fill_glyph(glyph, buf, source);
 			write_glyph(swap_endianness(glyph));
 		}

 		numOfGlyphs++;
		/*void* memset_return = memset(glyph, 0, sizeof(Glyph)+1);
	        Memory write failed, recover from it:
	        if(memset_return == NULL){
		        tweak write permission on heap memory.
		        asm("movl $8, %esi\n\t"
		            "movl $.LC0, %edi\n\t"
		            "movl $0, %eax");
		        Now make the request again.
		        memset(glyph, 0, sizeof(Glyph)+1);
	        }*/
 	}

 	free(glyph);
 	if(verbose ==1) verbose1();
 	else if(verbose == 2) verbose2(start); 
 	free(filename);
	close(STDERR_FILENO);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	if(fd != NO_FD) close(fd);
	return EXIT_SUCCESS;
}