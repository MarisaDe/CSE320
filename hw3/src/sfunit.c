#include <criterion/criterion.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfmm.h"

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(sizeof(int));
    *x = 4;
    cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *pointer = sf_malloc(sizeof(short));
    sf_free(pointer);
    pointer = pointer - 8;
    sf_header *sfHeader = (sf_header *) pointer;
    cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
    sf_footer *sfFooter = (sf_footer *) (pointer - 8 + (sfHeader->block_size << 4));
    cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");
}

Test(sf_memsuite, PaddingSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *pointer = sf_malloc(sizeof(char));
    pointer = pointer - 8;
    sf_header *sfHeader = (sf_header *) pointer;
    cr_assert(sfHeader->padding_size == 15, "Header padding size is incorrect for malloc of a single char!\n");
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(4);
    memset(x, 0, 4);
    cr_assert(freelist_head->next == NULL);
    cr_assert(freelist_head->prev == NULL);
}


Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *x = sf_malloc(4);
    void *y = sf_malloc(4);
    memset(y, 0xFF, 4);
    sf_free(x);
    cr_assert(freelist_head == x-8);
    sf_free_header *headofx = (sf_free_header*) (x-8);
    sf_footer *footofx = (sf_footer*) (x - 8 + (headofx->header.block_size << 4)) - 8;

    sf_blockprint((sf_free_header*)((void*)x-8));
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
    cr_assert(headofx->header.block_size << 4 == 32);
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);
    cr_assert(footofx->block_size << 4 == 32);
}

/*
//############################################
// STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
// DO NOT DELETE THESE COMMENTS
//############################################
*/


//Check to see if coalescing to the right only works.
Test(sf_memsuite, Coalesce_Right, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *x = sf_malloc(54);
    void *y = sf_malloc(54);
    //memset(y, 0xFF, 54);
    sf_free(y);
    cr_assert(freelist_head == y-8);
    sf_free_header *headofy = (sf_free_header*) (y-8);
    sf_free_header *headofx = (sf_free_header*) (x-8);
    sf_footer *footofy = (sf_footer*) (y - 8 + (headofy->header.block_size << 4)) - 8;
    //sf_footer *footofx = (sf_footer*) (x - 8 + (headofx->header.block_size << 4)) - 8;

    sf_blockprint((sf_free_header*)((void*)y-8));
    // All of the below should be true if there was was right coalescing
    cr_assert(headofy->header.alloc == 0, "Second value should NOT still be allocated!\n");
    cr_assert(headofy->header.block_size << 4 == 4016);
    cr_assert(headofy->header.padding_size == 0);

    //old value of y
    cr_assert(footofy->alloc == 0, "Coalesced footer did not match its header!\n");
    cr_assert(footofy->block_size << 4 == 0);


    //x should still be allocated
    cr_assert(headofx->header.alloc == 1, "First value should still be allocated!\n");
    cr_assert(headofx->header.block_size << 4 == 80);
    cr_assert(headofx->header.padding_size == 10);

}

//Check to see if info stats are correct
Test(sf_memsuite, Info_Test, .init = sf_mem_init, .fini = sf_mem_fini) {

    int *value1 = sf_malloc(sizeof(int));
    sf_malloc(sizeof(long));
    sf_free(value1);
    sf_malloc(2);

    info keepTrack;
    sf_info(&keepTrack);
    cr_assert(keepTrack.allocations == 3, "# of allocs made is incorrect\n");
    cr_assert(keepTrack.frees == 1, "# of frees made is incorrect!\n");
    cr_assert(keepTrack.external == 4032, "# of external bytes incorrect!\n");
    cr_assert(keepTrack.internal == 30, "# of internal bytes incorrect!\n");



}