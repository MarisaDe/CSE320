#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "sfmm.h"

/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */

sf_free_header* freelist_head = NULL;
static unsigned int long internal = 0;
static unsigned int long external = 0;
static unsigned int long allocations = 0;
//static unsigned int long frees = 0;
static unsigned int long coalesces = 0;

/**
* This is a helper funcion for malloc. It allocates memory and moves the freelist_head only.
* 
 */

void initFree()
{
	freelist_head->header.padding_size = 0;
	freelist_head->header.alloc = 0;
	freelist_head->next = NULL;
	freelist_head->prev = NULL;
	freelist_head->header.block_size = 4096 >> 4;
}

void *sf_fillAlloc(int payload, int padding, int total, sf_free_header* temp)
{
    //printf("%s%d\n", "temp block size: ", temp->header.block_size<<4);
	//set allocated header block values
	sf_header* alloc_head = (sf_header*)((char*)temp);
	alloc_head->padding_size = padding;
	alloc_head->block_size = total >> 4;
	alloc_head->alloc = 1;

	//Creates a pointer to memory that we want to return
	void *mem;
  	//sets mem pointer to the alloc head block to return later.
	mem = alloc_head;
	printf("%s%p\n", "MEM: ", mem);

	printf("%s%d\n", "freelist block size: ", freelist_head->header.block_size<<4);
	//sets values for the alloc footer
	sf_footer *alloc_footer = (sf_footer*)((char*)alloc_head + payload + SF_HEADER_SIZE);
	alloc_footer->alloc = 1;
	alloc_footer->block_size = total >> 4;
	printf("%s%d\n", "alloc footer size: ", alloc_footer->block_size<<4);
	printf("%s%p\n", "footer address ", alloc_footer);
	printf("%s%d\n", "temp block size: ", freelist_head->header.block_size<<4);
	//Move the freelist_header pointer to the next free block
	//freelist_head = (sf_free_header*)((char*)alloc_footer + SF_FOOTER_SIZE);

	//set footer for freelist
	sf_footer *freelist_foot = (sf_footer*)((char*)freelist_head + (freelist_head->header.block_size <<4) - SF_FOOTER_SIZE);
	freelist_foot->alloc = 0;
	freelist_foot->block_size = freelist_head->header.block_size;
	printf("%s%p\n", "free header address ", freelist_head);
	printf("%s%p\n", "free footer address: ", freelist_foot);

	//Returns payload address by increasing the pointer by size of the header block.
	allocations++;
	internal += SF_HEADER_SIZE + SF_FOOTER_SIZE + padding;
	printf("%s%d\n", "padding: ", padding);
	external = freelist_head->header.block_size << 4;
	printf("%s%lu\n", "# of internal bytes: ", internal);
	printf("%s%lu\n", "# of free bytes (external): ", external);
	printf("%s%lu\n", "# of allocs: ", allocations);
	return (char*)mem + SF_HEADER_SIZE;



}

/**
* Looks at the footer to the left of the header to check if it's free.
* If the footer is alloc = 0 (free) and it has a block size, then we must
* coalesce it! 
*/
void *coalescePrevious(sf_free_header* traverseHeaders)
{
	sf_free_header* temp = traverseHeaders;
	printf("%s%p\n", "temp footer is: ", ((char*)temp + ((temp->header.block_size <<4) - SF_HEADER_SIZE)));
	traverseHeaders = sf_sbrk(1);

	//traverseHeaders is the new block which will be 4096 when expanding the heap.
	traverseHeaders->header.block_size = (4096 >> 4);
	printf("%s%d\n", "allocating traverseheader mem in sbrk..: ", traverseHeaders->header.block_size<<4);

	//Check to see if there is free space previously adjacent to the new space created.
	//sf_footer* checkFree = (sf_footer*)((char*)traverseHeaders - SF_FOOTER_SIZE); //Checks footer of previous
	sf_footer* checkFree = (sf_footer*)((char*)temp + ((temp->header.block_size <<4) - SF_HEADER_SIZE));

	//CHANGE THE FOOTER BACK TO TRAVERSEHEADERS LATER//

	printf("%s%p\n", "PRINT CHKFREE FOOTER: ", checkFree);

	//Free so we must coalesce
	if(checkFree->alloc == 0 && checkFree->block_size<<4)
	{			
		//Set new header by getting the old header on the left
		sf_free_header *alloc_free_h = (sf_free_header*)((char*)checkFree - ((checkFree->block_size <<4) + SF_HEADER_SIZE));  	//moves this to the header on the left
		alloc_free_h->header.alloc = 0;
		alloc_free_h->header.block_size = traverseHeaders->header.block_size + checkFree->block_size;

		printf("%s%d\n", "alloc_free_h size!: ", alloc_free_h->header.block_size << 4);

		//Set the new footer
		checkFree = (sf_footer*)((char*)(alloc_free_h) + ((alloc_free_h->header.block_size <<4) - SF_FOOTER_SIZE)); 
		checkFree->alloc = 0;
		checkFree->block_size = alloc_free_h->header.block_size;
		coalesces++;	

		//******** Set the new freelist_head **************
		//Set up next and previous for the free header (copy that of the freelist_head)
		alloc_free_h->next = freelist_head;
		alloc_free_h->prev = NULL;

		//Now set the freelist_head to this new free header.
		freelist_head->prev = alloc_free_h;
		freelist_head = alloc_free_h;

		printf("%s%d\n", "# of block size h : ", alloc_free_h->header.block_size <<4);
		printf("%s%d\n", "# of block size f : ", checkFree->block_size <<4);

		//Add the free bytes to the external variable and subtract from internal.
		//external+= (alloc_free_h->header.block_size <<4);
		//internal-= (SF_HEADER_SIZE + SF_FOOTER_SIZE);   //somehow include PADDING
		//printf("%s%lu\n", "# of external: ", external);	
		//printf("%s%lu\n", "# of internal: ", internal);		

		//Reset traverseHeaders because the list changed.
		printf("%s%p\n", "PRINT CHK FOOTER: ", checkFree);
		return freelist_head;
	}
return NULL;
}


/**
* This is your implementation of malloc. It creates dynamic memory which
* is aligned and padded properly for the underlying system. This memory
* is uninitialized.
* @param size The number of bytes requested to be allocated.
* @return If successful, the pointer to a valid region of memory to use is
* returned, else the value NULL is returned and the ERRNO is set  accordingly.
* If size is set to zero, then the value NULL is returned.
*
*
* Step 1: Check to see if the freelist_head is NULL.
*     -> If NULL, get the sbrk pointer and set the freelist_head to that.
*	  -> Update the freelist_head stuct block to be allocated by the request.
*	  -> Move the freelist_head to point at the next free space available.
* Step 2: If freelist_head is NOT NULL, compare that block size to the requested size.
* 	  -> If the freelist_head size is smaller, look at the next block. Repeat until end of list.
			-> If the end of the list is reached, use sbrk to get more heap data.
				->If sbrk returns void -1, then return NULL since we ran out of space.
			    ->If we can use sbrk, add the new space as the next block in the list. 
*/
void *sf_malloc(size_t size){

// BASE CASES //////////////////////////////////////////////////////////////

if (size == 0) return NULL;
if (size > 16384){		//If size > 4 pages, error occurs.
	errno = EINVAL;
	return NULL;
}

//Initialize useful variables
int total = size + SF_HEADER_SIZE + SF_HEADER_SIZE;
int padding, payload = 0;
sf_free_header* temp;

//Account for padding and total size of the block if not divisible by 16
if(total % 16 !=0)
{	
	padding = total%16;
	padding = 16 - padding;
	total += padding;
	payload = total - (SF_HEADER_SIZE + SF_HEADER_SIZE);
}
//Account if it is divisible by 16
else
{
	padding = 0;
	payload = total - (SF_HEADER_SIZE + SF_HEADER_SIZE);
}
	
//If there are no free header blocks, create space for it./////////////////////////////////////////////
if(freelist_head == NULL){
	freelist_head = sf_sbrk(1);
	while((int long)freelist_head %8 != 0)
	{
		(char*)freelist_head++;
	}

	initFree();

	temp = freelist_head;
	printf("%s%d\n", "temp block size: ", temp->header.block_size<<4);
	freelist_head = (sf_free_header*)((char*)freelist_head + total);

	freelist_head->header.alloc = temp->header.alloc;
	freelist_head->header.block_size = temp->header.block_size - (total>>4);
	printf("%s%d\n", "freelist: ", freelist_head->header.block_size<<4);

	return sf_fillAlloc(payload, padding, total, temp);
}

//Freelist_head has an address. Check the list for space ///////////////////////////////////////////// 
else
{
	sf_free_header *traverseHeaders = freelist_head;
	printf("%s\n", "Freelist_head is not null.");

////////Found space somwhere in the freelist to allocate memory //////////////////////////////////////
while(traverseHeaders!= NULL){	

	if(total <= traverseHeaders->header.block_size<<4)
	{
		sf_free_header* next = traverseHeaders->next;
		printf("%s\n", "I REMALLOCED OH JOY");

		//Move the freelist_head next to the allocated space (which will be temp)
		//Update #of bytes in that block
		freelist_head = (sf_free_header*)((char*)traverseHeaders + total);
		freelist_head->header.alloc = 0;
		freelist_head->header.block_size = traverseHeaders->header.block_size - (total>>4);

		if(traverseHeaders->prev != NULL)traverseHeaders->prev->next = traverseHeaders->next;   //set previous block to current's next block
		if(traverseHeaders->next != NULL)traverseHeaders->next->prev = traverseHeaders->prev;   //set next block to the current's previous block

		printf("%s%d\n", "freelist_head block size: ", traverseHeaders->header.block_size <<4);
		freelist_head->prev = NULL;
		if(traverseHeaders->next != NULL)
		{
			freelist_head->next = next;
			next->prev = freelist_head;
		}
		//FIX THIS PREV SITUATION OR SOMESHIT
		freelist_head->prev = NULL;
		return sf_fillAlloc(payload, padding, total, traverseHeaders);
	}

////////Went through whole list and no space was adequate. Must get more from heap. //////////////////////////////////////	
	else if(traverseHeaders->next == NULL )
		{
			while(size > traverseHeaders->header.block_size <<4)
			{	
				traverseHeaders = coalescePrevious(traverseHeaders);									
			}

			//Already coalseced if necessary. Now add divide up this free block.
			return sf_malloc(size);
		}

			traverseHeaders = traverseHeaders->next;
		}
	}
	return NULL;
}
/**
* Marks a dynamically allocated region as no longer in use.
* Adds the newly freed block to the free list.
* @param ptr Address of memory returned by the function sf_malloc.
*/

void sf_free(void *ptr){

	if(ptr == NULL)
	{
		errno = EINVAL;
		return;
	}
	//Must check the bits of that address to make sure that memory was allocated

	ptr -= SF_HEADER_SIZE;   //ptr is a payload so we must subtract it to get the header size
	sf_header* alloc_h  = (sf_header*)(ptr);
	sf_footer* alloc_f  = (sf_footer*)(ptr + alloc_h->block_size - SF_FOOTER_SIZE);
	sf_free_header* alloc_free_h = ptr;

	//The address given was allocated! We must free it now.
	if(alloc_h->alloc == 1 && alloc_h->block_size <<4 >= 16 )
	{

		alloc_h += ((alloc_h->block_size <<4) - SF_HEADER_SIZE);

		//******* COALESCE RIGHT *************
		if(alloc_h->alloc == 0 && alloc_h->block_size <<4 >= 16)
		{

			//Set new header
			alloc_free_h = (sf_free_header*)((char*)alloc_h);  //moves this to the header on the right
			alloc_h = ptr; 			 //reset the original header to the header of block we currently wanna free
			alloc_free_h->header.alloc = 0;
			alloc_free_h->header.block_size += alloc_h->block_size;

			//Set the new footer
			alloc_f = (sf_footer*)(((char*)alloc_free_h) + (alloc_free_h->header.block_size<<4) - SF_FOOTER_SIZE); 
			alloc_f->alloc = 0;
			alloc_f->block_size = alloc_free_h->header.block_size;
			coalesces++;
			printf("%s%lu\n", "# of coalesces: ", coalesces);

		}

		alloc_f = (sf_footer*)(ptr - SF_HEADER_SIZE);

		//******* COALESCE LEFT *************
		if(alloc_f->alloc == 0 && alloc_f->block_size <<4 >= 16)
		{

			//Set new header
			alloc_free_h = ptr - ((alloc_f->block_size <<4) + SF_HEADER_SIZE);  	//moves this to the header on the left
			alloc_h = ptr; 			 							//reset the original header to the header of block we currently wanna free
			alloc_free_h->header.alloc = 0;
			alloc_free_h->header.block_size = alloc_h->block_size + alloc_f->block_size;

			//Set the new footer
			alloc_f = (sf_footer*)(((char*)alloc_free_h) - (alloc_free_h->header.block_size<<4) + SF_FOOTER_SIZE); 
			alloc_f->alloc = 0;
			alloc_f->block_size = alloc_free_h->header.block_size;
			coalesces++;
			printf("%s%lu\n", "# of coalesces: ", coalesces);

		}


		//******** NO COALESCING **************
		else
		{
			//Replace current header with a free header
			alloc_h = ptr;
			alloc_free_h = ptr;
			alloc_free_h->header.alloc = 0;
			alloc_free_h->header.block_size = alloc_h->block_size;

			//Free the footer
			alloc_f = ptr + (alloc_free_h->header.block_size <<4) - SF_FOOTER_SIZE;
			alloc_f->alloc = 0;
			alloc_f->block_size = alloc_free_h->header.block_size;
		}

		//******** Set the new freelist_head **************
		//Set up next and previous for the free header (copy that of the freelist_head)
		alloc_free_h->next = freelist_head;
		alloc_free_h->prev = NULL;

		//Now set the freelist_head to this new free header.
		freelist_head->prev = alloc_free_h;
		freelist_head = alloc_free_h;


		//Add the free bytes to the external variable and subtract from internal.
		external+= alloc_free_h->header.block_size << 4;
		internal-= alloc_free_h->header.block_size << 4;
		printf("%s%lu\n", "# of external: ", external);

	}

}

/**
 * Resizes the memory pointed to by ptr to be size bytes.
 * @param ptr Address of the memory region to resize.
 * @param size The minimum size to resize the memory to.
 * @return If successful, the pointer to a valid region of memory to use is
 * returned, else the value NULL is returned and the ERRNO is set accordingly.
 *
 * A realloc call with a size of zero should return NULL and set the ERRNO
 * accordingly.
 */
void *sf_realloc(void *ptr, size_t size){
  return NULL;
}

/**
 *  This function will copy  the correct values to the fields
 *  in the memory info struct.
 *  @param meminfo A pointer to the memory info struct passed
 *  to the function, upon return it will containt the calculated
 *  for current fragmentation
 *  @return If successful return 0, if failure return -1
 */
int sf_info(info* meminfo){

if(meminfo == NULL) return 0;

else
{	
	meminfo->allocations = allocations;
	meminfo->internal = internal;
	meminfo->external = external;
	//meminfo->frees = frees;
	meminfo->coalesce = coalesces;
  	return 0;
  }
}
