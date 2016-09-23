#include "utfconverter.h"

char* filename;
endianness source;
endianness conversion;

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
 	int c;
 	/*option_index = 0;*/
 	char* endian_convert;
 	filename = calloc(strlen(argv[argc-1])+1,sizeof(char));

 	if(argc > 1 && strcmp(argv[1],"-h") !=0 && strcmp(argv[1],"-u") !=0)
 	{
 		print_help();
 	}

 	if((c = getopt(argc, argv, "hu:")) != -1)
	{
		switch(c){ 
 			case 'h':
 				print_help();
 				break;
 			case 'u':
 				endian_convert = argv[optind-1];
 				break;
 			default:
 				fprintf(stderr, "Unrecognized argument.\n");
 				print_help();
 		}
 	}
 	else {
 			fprintf(stderr, "Invalid argument.\n");
 			print_help();
 	}

if(endian_convert == NULL)
 	{
 		fprintf(stderr, "OUT_ENC not given.\n");
 		print_help();
 	}

 	if(strcmp(endian_convert, "16LE") == 0)
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

 	/*optind must be less than all args AND there must be two more parameters after -u. (endianness and filename)*/
 	if(optind < argc)
 	{
 		strcpy(filename, argv[optind]);
 	} 
 	else 
 	{
 		fprintf(stderr, "Filename not given.\n");
 		print_help();
 		quit_converter(NO_FD); 
 	}

 	return;
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
	free(filename);
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
	/*After calling parse_args(), filename and conversion should be set.*/
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
		print_help();
	}

	unsigned int buf[2] = {0,0};

	Glyph* glyph = malloc(sizeof(Glyph)); 

	/*Handle BOM bytes for UTF16 specially.*/
    /* Read our values into the first and second elements.*/
	if((rv = read(fd, &buf[0], 1)) == 1 &&  (rv = read(fd, &buf[1], 1)) == 1)
	{ 
		if(buf[0]== 0xff000000 && buf[1] == 0xfe000000)
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
 			fprintf(stderr, "File has no BOM.\n");
 			print_help();
 			quit_converter(NO_FD); 
 			return EXIT_FAILURE;
 		}
 		void* memset_return = memset(glyph, 0, sizeof(Glyph)+1);
 		/*Memory write failed, recover from it:*/
 		if(memset_return == NULL)
 		{
 			/*tweak write permission on heap memory.
 			asm("movl $8, %esi\n\t"
 			    "movl $.LC0, %edi\n\t"
 			    "movl $0, %eax");
 			Now make the request again.*/
 			memset(glyph, 0, sizeof(Glyph)+1);

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

	free(&glyph->bytes); 
 	free(filename);
	quit_converter(NO_FD);
	return 0;
}