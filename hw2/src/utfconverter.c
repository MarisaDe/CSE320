#include "utfconverter.h"

char* filename;
char* OUT_ENC;
bool isThereOUT_ENC = false;
endianness source;
endianness conversion;
int verbose;
int numOfGlyphs = 0;
int numOfSurrogates = 0;
int numOfAscii = 0;
int fd;

 Glyph* swap_endianness(Glyph* glyph)
 {
 	if(!glyph->surrogate)
 	{
 		/*Surrogates do the same thing as non-surrogates.*/
 		unsigned int swap = glyph->bytes[0];
 		glyph->bytes[0] = glyph->bytes[1];
 		glyph->bytes[1] = swap;
 	}
 	else
 	{
 		/*Surrogates do the same thing as non-surrogates.*/
 		unsigned int swap = glyph->bytes[0];
 		glyph->bytes[0] = glyph->bytes[1];
 		glyph->bytes[1] = swap;
 		swap = glyph->bytes[2];
 		glyph->bytes[2] = glyph->bytes[3];
 		glyph->bytes[3] = swap;
 	}



 	glyph->end = conversion;
 	return glyph;
 }



Glyph* fill_glyph(Glyph* glyph,unsigned int data[2],endianness end) 
{
 	unsigned int bits = 0; 
 	/*These would be non-surragate pairs.*/
 	if(end == BIG)
 	{
 		bits |= (data[0] + (data[1] << 8));
 		glyph->bytes[0] = data[1];
 		glyph->bytes[1] = data[0];

 	}
 	else if(end == LITTLE)
 	{
 		glyph->bytes[0] = data[0];
 		glyph->bytes[1] = data[1];
 		bits |= ((data[0]<<8) + data[1]);

 	}

 	glyph->surrogate = false;
 	bits = 0;
 	if(end == LITTLE) bits |= (data[0] + (data[1] << 8));
 	else if(end == BIG) bits |= ((data[0]<<8) + data[1]);
	if(bits >= 0xD800 && bits <= 0xDBFF){
		if(read(fd, &data[0], 1) == 1 && read(fd, &data[1], 1) == 1){
			bits = 0;
			if(end == LITTLE) bits |= (data[0] + (data[1] << 8));
 			else if(end == BIG) bits |= ((data[0]<<8) + data[1]);
			/* Check low surrogate pair.*/
			if(bits >= 0xD300 && bits <=0xDFFF){ 
			 	numOfSurrogates++;
 				glyph->surrogate = true; 

			} else { 
 				glyph->surrogate = false;
 				lseek(fd, -OFFSET, SEEK_CUR);
 			}
		}
	}

	if(glyph->surrogate)
	{
		if(end == LITTLE)
		{
			glyph->bytes[2] = data[0];
			glyph->bytes[3] = data[1];
		}
		else if(end == BIG)
		{
			glyph->bytes[2] = data[1];
			glyph->bytes[3] = data[0];
		}
	}

	if(bits <= 127) numOfAscii++; 
  	glyph->end = end;
  	return glyph;
}

void write_glyph(Glyph* glyph)
 {
 	if(isThereOUT_ENC)
 	{
 		int fdo = open(OUT_ENC, O_CREAT | O_WRONLY | O_APPEND, 0666);
 		if(glyph->surrogate)
 		{
 			write(fdo, glyph->bytes, SURROGATE_SIZE);
 		} else 
 		{
 			write(fdo, glyph->bytes, NON_SURROGATE_SIZE);
 		}
 	}
 	else
 	{
 		if(glyph->surrogate)
 		{
 			write(STDOUT_FILENO, glyph->bytes, SURROGATE_SIZE);
 		} else 
 		{
 			write(STDOUT_FILENO, glyph->bytes, NON_SURROGATE_SIZE);
 		}
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
 					filename = calloc(strlen(argv[optind])+1,sizeof(char));
 					strcpy(filename, argv[optind]);
 				}
 				else 
 				{	
 					fprintf(stderr, "Filename not given.\n");
 					free(filename);
 					print_help();
 					quit_converter(NO_FD); 
 				}
 				if(argc- optind > 1)
 				{
 					isThereOUT_ENC = true;
 					OUT_ENC = calloc(strlen(argv[optind+1])+1,sizeof(char));
 					strcpy(OUT_ENC, argv[optind+1]);
 				}

 				break;
 			default:
 				fprintf(stderr, "Unrecognized argument.\n");
 				quit_converter(NO_FD);
 				break;
 		}

 	}
if(verbose > 2) verbose = 2;
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

	char actual[1048]; 
    realpath(filename, actual);  


	fprintf(stderr,"\t%s%f%s\n","Input file size: ",kb, " kb");
	fprintf(stderr,"\t%s%s\n","Input file path: ", actual);
	if(source == LITTLE) 	fprintf(stderr,"\t%s","Input file encoding: UTF-16LE\n");
	else if(source == BIG) 	fprintf(stderr,"\t%s","Input file encoding: UTF-16BE\n");
	else if(source == EIGHT) 	fprintf(stderr,"\t%s","Input file encoding: UTF-8\n");
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
	float asciiPercent = ((float)numOfAscii/(float)numOfGlyphs)*100;
	int surrogatePercent = ((float)numOfSurrogates/(float)numOfGlyphs)*100;

	fprintf(stderr,"\t%s%f%s%f%s%f\n","Reading: real=",cpu_time_used," user=",cpu_time_used," sys=",cpu_time_used);
	fprintf(stderr,"\t%s%f%s%f%s%f\n","Converting: real=",cpu_time_used," user=",cpu_time_used," sys=",cpu_time_used);
	fprintf(stderr,"\t%s%f%s%f%s%f\n","Writing: real=",cpu_time_used," user=",cpu_time_used," sys=",cpu_time_used);
	fprintf(stderr,"\t%s%f%s\n","ASCII: ", asciiPercent,"%");
	fprintf(stderr,"\t%s%i%s\n","Surrogates: ",surrogatePercent, "%");
	fprintf(stderr,"\t%s%i\n","Glyphs: ",numOfGlyphs);
}

void print_help() {
	int i;
	for(i = 0; i < 4; i++){
		fprintf(stderr,"%s", USAGE[i]); 
	}
	quit_converter(NO_FD);
}

void quit_converter(int fd)
{
	close(STDERR_FILENO);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	if(fd != NO_FD)
		close(fd);
	exit(0);
	EXIT_FAILURE;
	/*Ensure that the file is included regardless of where we start compiling from.*/
}

int main(int argc, char** argv)
{
	/*Start the clock*/
	clock_t start = clock();
	/*After calling parse_args(), filename and conversion should be set.*/
	parse_args(argc, argv);

	if(!isThereOUT_ENC) OUT_ENC = NULL;

	/*Get realpath of the file.*/
  	char actual[1048]; 
    realpath(filename, actual); 

	fd = open(actual, O_RDONLY); 
	/*rv is for read*/
	int rv = 0;

	if(fd < 0)
	{
		fprintf(stderr, "Not a valid filename.\n"); 
 		free(filename);
 		if(isThereOUT_ENC) free(OUT_ENC);
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
			/*include BOM in # of glyphs*/
			numOfGlyphs++;
 		} 
		else if(buf[0] == 0xfe && buf[1] == 0xff)
		{
 			/*file is big endian FEFF*/
 			source = BIG;
 			/*include BOM in # of glyphs*/
 			numOfGlyphs++;
 		} 
 		else if(buf[0]== 0xef && buf[1] == 0xbb)
 		{
 			if((rv = read(fd, &buf[0], 1)) == 1){
				if(buf[0]== 0xbf)
				{
					/*file is UTF-8*/
					source = EIGHT;
					numOfGlyphs++;
				} 
				else
				{
					free(&glyph->bytes); 
 					free(filename);
 					fprintf(stderr, "File has no BOM.\n");
 					print_help();
 					quit_converter(NO_FD); 	
 				}	
 			}	
			else{
				free(&glyph->bytes); 
 				free(filename);
 				fprintf(stderr, "File has no BOM.\n");
 				print_help();
 				quit_converter(NO_FD); 
 			}	
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
 			memset(glyph, 0, sizeof(Glyph));
 		}
	}

	/*Accounts for endianess and swaps it*/
	if(conversion == source)
	{
		write_glyph(fill_glyph(glyph, buf, source));
	}
	else if(conversion != source)
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
 	}

 	free(glyph);
 	if(verbose ==1) verbose1();
 	else if(verbose == 2) verbose2(start);
 	if(isThereOUT_ENC) free(OUT_ENC); 
 	free(filename);
	close(STDERR_FILENO);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	if(fd != NO_FD) close(fd);
	return EXIT_SUCCESS;
}