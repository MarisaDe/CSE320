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

//static char page[4096];
//static char maxsize[16384];

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
if (size > 4096){
	errno = EINVAL;
	return NULL;
}

//Creates a pointer to memory that we want to return
void *mem;

//If there is no free header blocks, create space for it.
if(freelist_head == NULL){
	freelist_head = sf_sbrk(0);
	while((int long)freelist_head %8 != 0)
	{
		(char*)freelist_head++;
	}
	memset(freelist_head, 0, sizeof(*freelist_head));

}

	int total = size + SF_HEADER_SIZE + SF_HEADER_SIZE;
	printf("%s%d\n", "size: ", total);
	int padding = 0;

	//Account for padding and total size of the block
	if(total % 16 !=0)
	{	
		padding = total%16;
		padding = 16 - padding;
		total += padding;
	}

	//set values of freelist_head
	freelist_head->header.padding_size = padding;
	freelist_head->header.block_size = total >>4;
	freelist_head->header.alloc = 1;


  	//gets heap breakpoint without moving it. Will always be a multiple of 8.
	mem = freelist_head;
	printf("%s%p\n", "MEM: ", mem);
	//struct sf_header test;
	//test.alloc = 1;
	//struct sf_header *headerptr = &test;
	//*(unsigned int long *)(mem) = (unsigned int long)(headerptr);
	//printf("%s%p\n", "Header PTR: ", headerptr);

	//Returns payload address by increasing the pointer by size of block.
	return (char*)mem + 8;
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
