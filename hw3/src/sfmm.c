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
//static unsigned int long coalesces = 0;

/**
* This is a helper funcion for malloc. It allocates memory and moves the freelist_head only.
* 
 */
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
	//Update all free space

	temp = freelist_head;
	printf("%s%d\n", "temp block size: ", temp->header.block_size<<4);
	freelist_head = (sf_free_header*)((char*)freelist_head + total);

	freelist_head->header.alloc = temp->header.alloc;
	freelist_head->header.block_size = temp->header.block_size - (total>>4);
	printf("%s%d\n", "freelist: ", freelist_head->header.block_size<<4);

	return sf_fillAlloc(payload, padding, total, temp);
}

//freelist_head must have some address. Compare needed size to size of free block to see if we can use it. 
else
{
	printf("%s\n", "Freelist_head is NOT NULL ");
	printf("%s%d\n", "freelist block size!!!: ", freelist_head->header.block_size<<4);
	//We found space to allocate memory in the freelist_head!
	if(total <= freelist_head->header.block_size<<4)
	{
		temp = freelist_head;
		freelist_head = (sf_free_header*)((char*)freelist_head + total);
		freelist_head->header.alloc = temp->header.alloc;
		freelist_head->header.block_size = temp->header.block_size - (total>>4);
		return sf_fillAlloc(payload, padding, total, temp);
	}

	//We must search if there are any other free blocks in the the freelist linked list then compare sizes. 
	else{

		sf_free_header *traverseHeaders = freelist_head;

		//the freelist_head was the only element in the list. We already checked for size so we know it's too small.
		//Expand ze heap here!
		if(traverseHeaders->next == NULL)
		{
			while(size > freelist_head->header.block_size <<4)
			{
				temp = freelist_head;
				freelist_head= sf_sbrk(1);
				freelist_head->header.block_size = temp->header.block_size+ (4096 >> 4);
				printf("%s%d\n", "allocating mem in sbrk..: ", freelist_head->header.block_size<<4);
			}
				temp = freelist_head;
				freelist_head->header.alloc = 1;
				freelist_head->header.block_size = freelist_head->header.block_size - (total>>4);
				printf("%s%d\n", "temp size: ", temp->header.block_size<<4);
				return sf_fillAlloc(payload, padding, total, temp);

		}
		//There is something next in our free linked list.
		while(traverseHeaders!= NULL)
		{
				//We can fit our data into that free block!
				if(total <= freelist_head->next->header.block_size<<4)
				{
						//sf_free_header* temp = freelist_head;
						//printf("%s%d\n", "temp block size: ", temp->header.block_size<<4);
						//freelist_head = (sf_free_header*)((char*)freelist_head + total);

						//freelist_head->header.alloc = temp->header.alloc;
						//freelist_head->header.block_size = temp->header.block_size - (total>>4);
						//printf("%s%d\n", "freelist: ", freelist_head->header.block_size<<4


					//ALLOCATE MEMORY HEREEEEEE
				}
				//Last element in the list AND not enough space. We must expand the heap.
				else if(traverseHeaders->next == NULL )
				{
					while(size > traverseHeaders->header.block_size <<4)
					{
						//printf("%s%d\n", "allocating mem in sbrk..: ", freelist_head->header.block_size<<4);
						temp = traverseHeaders;
						traverseHeaders->next = sf_sbrk(1);
						traverseHeaders->next->header.block_size = temp->header.block_size+ (4096 >> 4);
						printf("%s%d\n", "allocating traverseheader mem in sbrk..: ", freelist_head->header.block_size<<4);
					}

					traverseHeaders->next = (sf_free_header*)((char*)freelist_head + total);
					traverseHeaders->next->header.alloc = 1;
					traverseHeaders->next->header.block_size = temp->header.block_size - (total>>4);
					return sf_fillAlloc(payload, padding, total, temp);
					//Set allocated memory to new free block and return that payload address.
					//return sf_fillAlloc(payload,padding,total);


				}

			traverseHeaders = traverseHeaders->next;
		}

	return NULL;	
	}
}
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

	//The address given was allocated! We must free it now.
	if(alloc_h->alloc == 1)
	{
		//Check the (left) adjacent footer to see if that's a free block.
		alloc_f = (sf_footer*)(ptr + SF_HEADER_SIZE);

		//The left is free so we must coalesce it
		//** CASE 3 or 4 at this point **
		if(alloc_f->alloc == 0){



		}

		//Now we must check the block on the right
		//The left block must be allocated so we ignore it and check the header block on the right.
		else
		{
			//Update alloc header address to the header to its right.
			alloc_h += (alloc_h->block_size - SF_HEADER_SIZE);

			//The right is free so we must coalesce it
			//** CASE 2 ** since we checked for left prior and it is already allocated.
			if(alloc_h->alloc == 0)
			{
				//
			}

			//** CASE 1 = neither blocks adjacent to it are free **
			//Add this block as the freelist_head
			else
			{
				//Replace current header with a free header
				alloc_h = ptr;
				sf_free_header* alloc_free_h = ptr;
				alloc_free_h->header.alloc = 0;
				alloc_free_h->header.block_size = alloc_h->block_size;

				//Free the footer
				alloc_f = ptr + alloc_free_h->header.block_size - SF_FOOTER_SIZE;
				alloc_f->alloc = 0;
				alloc_f->block_size = alloc_free_h->header.block_size;

				//Set up next and previous for the free header (copy that of the freelist_head)
				alloc_free_h->next = freelist_head->next;
				alloc_free_h->prev = freelist_head->prev;

				//Now set the freelist_head to this new free header.
				freelist_head = alloc_free_h;

				//Add the free bytes to the external variable and subtract from internal.
				external+= alloc_free_h->header.block_size;
				internal-= alloc_free_h->header.block_size;
			}

		}

		return;
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
	//meminfo->coalesces= coalesces;
  	return 0;
  }
}
