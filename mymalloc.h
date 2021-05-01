/**
 * File:        mymalloc.h
 * Author:      Mohammad S Kharboutli
 * Date:        25 OCT 2020
 * Course:      CS214: Systems Programming
 * University:  Rutgers--New Brunswick
 * 
 * Summary of File:
 *     This include file contains all necessary dependencies for
 *     the mymalloc() and myfree() library functions. It also defines
 *     the metadata struct and declares our simulated memory. The macro
 *     allows us to call malloc(1) but really use mymalloc(3) with __FILE__
 *     and __LINE__ preprocessor functionalities.
 */

#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stdlib.h>
#include <stdio.h>

void* mymalloc(size_t size, char* file, int line);
void myfree(void* my_ptr, char* file, int line);
void print_view(void);

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

struct metadata {
    short block_size;
    short prev_block_size;
    unsigned char free;
};
typedef struct metadata metadata;
static char sim_mem[4096];

#endif