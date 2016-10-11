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


/**
* This is a helper funcion for malloc
* 
 */
void *sf_fillAlloc(int payload, int padding, int total)
{
	//set allocated header block values
	sf_header* alloc_head = (sf_header*)((char*)freelist_head);
	alloc_head->padding_size = padding;
	alloc_head->block_size = total >> 4;
	alloc_head->alloc = 1;

	//Creates a pointer to memory that we want to return
	void *mem;
  	//sets mem pointer to the alloc head block to return later.
	mem = alloc_head;
	printf("%s%p\n", "MEM: ", mem);

	//sets values for the alloc footer
	sf_footer *alloc_footer = (sf_footer*)((char*)freelist_head + payload + SF_HEADER_SIZE);
	memset(alloc_footer, 0, sizeof(*alloc_footer));
	alloc_footer->alloc = 1;
	alloc_footer->block_size = total >> 4;
	printf("%s%p\n", "footer address ", alloc_footer);

	sf_free_header* freetemp = (sf_free_header*)((char*)alloc_footer + SF_FOOTER_SIZE);
	freetemp->header.block_size = freelist_head->header.block_size;
	printf("%s%d\n", "FREETEMP block size: ", freetemp->header.block_size<<4);
	printf("%s%d\n", "freelist_head block size: ", freelist_head->header.block_size<<4);
	//Move the freelist_header pointer to the next free block
	freelist_head = (sf_free_header*)((char*)alloc_footer + SF_FOOTER_SIZE);

	//Reset the freelist_head values as if it was a free block
	freelist_head->header.block_size = (freetemp->header.block_size - alloc_head->block_size) >> 4;
	printf("%s%d\n", "freelistb size: ", freelist_head->header.block_size<<4);

	//Returns payload address by increasing the pointer by size of the header block.
	return (char*)mem + SF_HEADER_SIZE;



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

if (size == 0) return NULL;
if (size > 16384){		//If size > 4 pages, error occurs.
	errno = EINVAL;
	return NULL;
}

//Initialize useful variables
int total = size + SF_HEADER_SIZE + SF_HEADER_SIZE;
int padding, payload = 0;

//Account for padding and total size of the block
if(total % 16 !=0)
{	
	padding = total%16;
	padding = 16 - padding;
	total += padding;
	payload = total - (SF_HEADER_SIZE + SF_HEADER_SIZE);
}


//If there is no free header blocks, create space for it.
if(freelist_head == NULL){
	freelist_head = sf_sbrk(0);
	while((int long)freelist_head %8 != 0)
	{
		(char*)freelist_head++;
	}

	//initialize free block
	freelist_head->header.padding_size = 0;
	freelist_head->header.alloc = 0;
	freelist_head->next = NULL;
	freelist_head->prev = NULL;
	freelist_head->header.block_size = 4096 >> 4;
	printf("%s%d\n", "freelist_head block size: ", freelist_head->header.block_size<<4);
	return sf_fillAlloc(payload, padding, total);
}

//freelist_head must have some address. Compare needed size to size of free block to see if we can use it. 
else
{
	printf("%s\n", "Freelist_head is NOT NULL ");
	printf("%s%d\n", "freelist block size!!!: ", freelist_head->header.block_size);
	//We found space to allocate memory in the freelist_head!
	if(total <= freelist_head->header.block_size<<4)
	{
		printf("%s%d\n", "freelist block size!!!: ", freelist_head->header.block_size);
		return sf_fillAlloc(payload, padding, total);
	}

	//We must search if there are any other free blocks in the the freelist linked list then compare sizes. 
	else{

		sf_free_header *traverseHeaders = freelist_head;
		//There is something next in our free linked list.
		while(traverseHeaders->next != NULL)
		{
				//We can fit our data into that free block!
				if(total <= freelist_head->next->header.block_size<<4)
				{
					//ALLOCATE MEMORY HEREEEEEE
				}
			traverseHeaders = traverseHeaders->next;
		}
		//We ran out of elements in our linked list so we must request for more space on the heap.
		//initialize free block
		freelist_head->header.padding_size = 0;
		freelist_head->header.alloc = 0;
		freelist_head->next = NULL;
		freelist_head->prev = NULL;
		freelist_head->header.block_size = 0 >> 4;

		while(size > freelist_head->header.block_size<<4)
		{
			freelist_head= sf_sbrk();
			freelist_head->header.block_size += 4096 >> 4;
		} 
		//If bigger than 4 pages, return null!
		if(sf_sbrk(0) == (void *) -1)
		{
			return NULL;
		}
		//Set allocated memory to new free block and return that payload address.
		return sf_fillAlloc(payload,padding,total);
		}

	return NULL;	
	}
}

/**
* Marks a dynamically allocated region as no longer in use.
* Adds the newly freed block to the free list.
* @param ptr Address of memory returned by the function sf_malloc.
*/

void sf_free(void *ptr){

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
  return -1;
}
