#include "utfconverter.h"

char* filename;
endianness source;
endianness conversion;

// Glyph* swap_endianness(Glyph* glyph)
// {
// 	/* Use XOR to be more efficient with how we swap values. */
// 	glyph->bytes[0] ^= glyph->bytes[1];
// 	glyph->bytes[1] ^= glyph->bytes[0];
// 	if(glyph->surrogate){  /* If a surrogate pair, swap the next two bytes. */
// 		glyph->bytes[2] ^= glyph->bytes[3];
// 		glyph->bytes[3] ^= glyph->bytes[2];
// 	}
// 	//glyph->end = conversion;
// 	return glyph;
// }



Glyph* fill_glyph(Glyph* glyph,unsigned int data[2],endianness end, int* fd) 
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
 		printf("%s%x\n", "combined w/ BE: ",bits);
 		//glyph->bytes[0] = bits;
 		//glyph->bytes[1] = bits >> 2;
 		//printf("%s%x\n", "first G: ",glyph->bytes[0]);
 		//printf("%s%x\n", "second G :",glyph->bytes[1]);

 	}
 	else if(end == LITTLE)
 	{
 		bits |= ((data[0]<<8) + data[1]);
 		printf("%s%x\n", "combined w/ LE: ",bits);
 		//glyph->bytes[0] = bits;
 		//glyph->bytes[1] = bits << 2;
 		//printf("%s%x\n", "first G: ",glyph->bytes[0]);
 		//printf("%s%x\n", "second G :",glyph->bytes[1]);

 	}


 	/*Check for surrogate pairs */
 	if(bits > 0x10000 || bits == 0x10000)
 	{	
 		glyph->surrogate = true;
 		unsigned int high, low = 0;
 		bits = bits - 0x10000;
 		high = (bits >> 10);
 		high+= 0xD800;
 		low =(bits ^= 0x3FF);
 		low+= 0xDC00;
 		printf("%s%x\n", "surrogate high:", high);
 		printf("%s%x\n", "surrogate low:", low);
 		glyph->bytes[0] = data[high];
 		glyph->bytes[1] = data[low];

 	}
 	else
 		glyph->surrogate = false;


// 	/* Check high surrogate pair using its special value range.*/
//  	if(bits > 0x000F && bits < 0xF8FF)
//  	{ 
//  		if(read(*fd, &data[SECOND], 1) == 1 && read(*fd, &data[FIRST], 1) == 1)
//  		{
//  			bits = '0'; /* bits |= (bytes[0] + (bytes[1] << 8)) */
//  			if(bits > 0xDAAF && bits < 0x00FF)
// 			{ /* Check low surrogate pair.*/
//  				glyph->surrogate = false; 
//  				printf("%s\n", "No surrogate pair");
//  			}
//  			else 
//  			{
//  				lseek(*fd, -OFFSET, SEEK_CUR); 
//  				glyph->surrogate = true;
//  				printf("%s\n", "Surrogate pair found!");
//  			}
//  		}
//  	}
//  	if(!glyph->surrogate)
//  	{
//  		glyph->bytes[THIRD] = glyph->bytes[FOURTH] |= 0;
//  	} 
//  	 else 
//  	{
//  	 	glyph->bytes[THIRD] = data[FIRST]; 
//  	 	glyph->bytes[FOURTH] = data[SECOND];
// 	}
  	glyph->end = end;

  	return glyph;
 }

void write_glyph(Glyph* glyph)
 {
 	if(glyph->surrogate){
 		write(STDIN_FILENO, glyph->bytes, SURROGATE_SIZE);
 	} else {
 		write(STDIN_FILENO, glyph->bytes, NON_SURROGATE_SIZE);
 	}
 }

 void parse_args(int argc, char** argv)
 {
 	int option_index, c;
 	option_index = 0;
 	filename = NULL;
 	char* endian_convert = NULL;
 	//#include "struct.txt" 
	struct option long_options[] = 
	{
		{"h", optional_argument, 0, 'h'},
		{"u", required_argument, 0, 'u'},

	};


// 	/* If getopt() returns with a valid (its working correctly) 
// 	 * return code, then process the args! */
 	if((c = getopt_long(argc, argv, "hu", long_options, &option_index)) != -1)
 	{
 		switch(c){ 
 			case 'h':
 				print_help();
 				quit_converter(NO_FD);
 				break;
 			case 'u':
 				endian_convert = argv[optind];
 				break;
 			default:
 				fprintf(stderr, "Unrecognized argument.\n");
 				quit_converter(NO_FD);
 				break;
 		}

 	}

 	/*optind must be less than all args AND there must be two more parameters after -u. (endianness and filename)*/
 	if(optind < argc && argc-optind != 1)
 	{
 		filename = malloc(sizeof(argv[optind+1]));
 		strcpy(filename, argv[optind+1]);
 	} 
 	else 
 	{
 		fprintf(stderr, "Filename not given.\n");
 		print_help();
 		quit_converter(NO_FD);
 	}

 	if(endian_convert == NULL)
 	{
 		fprintf(stderr, "Converson mode not given.\n");
 		print_help();
 	}

 	if(strcmp(endian_convert, "16LE"))
 	{ 
 		conversion = LITTLE;
 	} 
 	else if(strcmp(endian_convert, "16BE"))
 	{
 		conversion = BIG;
 	} 
 	else 
 	{
 		quit_converter(NO_FD);
 	}
 	return;
}



void print_help() {
	for(int i = 0; i < 5; i++){
		printf("%s", USAGE[i]); 
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
	/* Ensure that the file is included regardless of where we start compiling from. */
	//#include "utfconverter.c"
}

int main(int argc, char** argv)
{
	/* After calling parse_args(), filename and conversion should be set. */
	parse_args(argc, argv);
	//print_help();

// 	int fd = open("rsrc/utf16le.txt", O_RDONLY); 
// 	/*rv is for read */
// 	int rv = 0;
// 	unsigned int buf[2] = {0,0};

// 	Glyph* glyph = malloc(sizeof(Glyph)); 

// 	/* Handle BOM bytes for UTF16 specially. 
//     * Read our values into the first and second elements. */
// 	if((rv = read(fd, &buf[0], 1)) == 1 &&  (rv = read(fd, &buf[1], 1)) == 1)
// 	{ 
// 		if(buf[0]== 0xff && buf[1] == 0xfe)
// 		{
// 			/*file is little endian FFFE*/
// 			source = LITTLE; 
// 			printf("%d", source);
//  		} 
// 		else if(buf[0] == 0xfe && buf[1] == 0xff)
// 		{
// // 			/*file is big endian FEFF*/
//  			source = BIG;
//  			printf("%d", source);
//  		} 
			
// 		else {
// // 			/*file has no BOM*/
//  			free(&glyph->bytes); 
//  			fprintf(stderr, "File has no BOM.\n");
//  			quit_converter(NO_FD); 
//  		}
//  		void* memset_return = memset(glyph, 0, sizeof(Glyph)+1);
//  		/* Memory write failed, recover from it: */
//  		if(memset_return == NULL)
//  		{
//  			 tweak write permission on heap memory. 
//  			asm("movl $8, %esi\n\t"
//  			    "movl $.LC0, %edi\n\t"
//  			    "movl $0, %eax");
//  			/* Now make the request again. */
//  			memset(glyph, 0, sizeof(Glyph)+1);

//  		}
// 	}
// 	printf("%s\n", "I got here");

// // 	Now deal with the rest of the bytes.
//  	while((rv = read(fd, &buf[0], 1)) == 1 &&  (rv = read(fd, &buf[1], 1)) == 1)
// 	{
//  		write_glyph(fill_glyph(glyph, buf, source, &fd));
// // 		void* memset_return = memset(glyph, 0, sizeof(Glyph)+1);
// // 	        /* Memory write failed, recover from it: */
// // 	        if(memset_return == NULL){
// // 		        /* tweak write permission on heap memory. */
// // 		        asm("movl $8, %esi\n\t"
// // 		            "movl $.LC0, %edi\n\t"
// // 		            "movl $0, %eax");
// // 		        /* Now make the request again. */
// // 		        memset(glyph, 0, sizeof(Glyph)+1);
// // 	        }
//  	}


// 	quit_converter(NO_FD);
// 	return 0;
}