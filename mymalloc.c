/**
 * File:        mymalloc.c
 * Author:      Mohammad S Kharboutli
 * Date:        25 OCT 2020
 * Course:      CS214: Systems Programming
 * University:  Rutgers--New Brunswick
 * 
 * Summary of File:
 *     This library acts as a malloc() library replacement. It includes
 *     both malloc() and free() functionalities, and uses a 4kB simulated
 *     memory space (defined as a static char array of size 4096). Much
 *     like with actual malloc() and free(), these functions include error
 *     handling and even extend some of those error handling functionalities.
 */

#include "mymalloc.h"
metadata *init = (metadata*)sim_mem;

/**
 * void print_view()
 * 
 * Description:
 *     Print view will move from block to block (only those that are active)
 *     and print out relevant data, displaying what our memory block looks like.
 *     Data includes 'free' flag, current block size, previous block size, the 
 *     value coressponding to both the metadata pointer as well as the data block
 *     pointer.
 * 
 * Parameters:
 *     None.
 * 
 * Returns:
 *     None.
 * 
 * Throws:
 *     None.
 */
void print_view() {
    int mem_ptr = 0;
    printf("---------------------------------------------\n");
    for(metadata* curr = init; curr->block_size != 0; 
        curr = (metadata*)(((char*)curr) + sizeof(metadata) + curr->block_size)) {
        printf("[%d] BLKPTR: %d | BLKSIZE: %d | FREE: %d | PREV BLK: %d", 
        mem_ptr, (int)(mem_ptr+sizeof(metadata)), curr->block_size, 
        curr->free, curr->prev_block_size);

        mem_ptr += curr->block_size + sizeof(metadata);
        printf("\n");
    }
    printf("---------------------------------------------\n");
}


/**
 * void exit_malloc(int code, char* file, int line)
 * 
 * Description:
 *     This function take in an integer code corresponding to a reason for error
 *     and will print the corresponding error. It will also give relevant file
 *     and line information using the __FILE__ and __LINE__ preprocessor 
 *     directives.
 * 
 * Parameters:
 *     code: integer corresponding to a error--between 1 and 5
 *     file: the current file, as given by __FILE__
 *     line: the current line, as given by __LINE__
 * 
 * Returns:
 *     None.
 * 
 * Throws:
 *     None.
 */
void exit_malloc(int code, char* file, int line) {
    printf("%s:%d [ERROR]: FAILED WITH CODE %d:", file, line, code);
    switch (code) {
    case 1:
        printf("\n\tINPUT POINTER OUT OF BOUNDS\n");
        break;
    case 2:
        printf("\n\tINPUT POINTER IS NOT VALID POINTER\n"); 
        break;
    case 3:
        printf("\n\tINPUT POINTER HAS ALREADY BEEN FREED\n");
        break;
    case 4:
        printf("\n\tMEMORY TOO SATURATED, NO SPACE FOR ALLOCATION\n");
        break;
    case 5:
        printf("\n\tBLOCK SIZE REQUESTED IS INVALID VALUE\n");
        break;
    default:
        printf("\n\tTOTAL AND ABSOLUTE CATASTROPHIC FAILURE\n");
        break;
    }
}


/**
 * void* mymalloc(size_t size, char* file, int line)
 * 
 * Summary:
 *     Returns a pointer in simulated memory to a block of usable memory of the desired size.
 * 
 * Description:
 *     This function is a malloc() replacement--using simulated memory of 4096 bytes
 *     the function segments wanted memory using a metadata structure and returns
 *     the pointer of allocated memory to the user. The function keeps track of
 *     previous and next metadata blocks using arithmetic to save on space.
 * 
 * Parameters:
 *     size: a value between 1 and 4090 corresponding to the number of wanted
 *           bytes to be allocated for the user.
 *     file: the current file, as given by __FILE__.
 *     line: the current line, as given by __LINE__.
 * 
 * Returns:
 *     A pointer of "void*" type, to be used by the user through casting.
 * 
 * Throws:
 *     INVALID SIZE ERROR: If size is less than or equal to 0.
 *     SATURATION ERROR: If size is greater than the available memory.
 */
void* mymalloc(size_t size, char* file, int line) {
   if((int)size <= 0) {
       exit_malloc(5, file, line);
       return NULL;
   }
   if(!init->block_size) {
        metadata alloc = {4096 - sizeof(metadata), 0, 1};
        *init = alloc;
    }
    int offset = 0;
    for(metadata *curr = init; curr->block_size != 0; curr = (void*)curr + offset) {
        if(curr->block_size > size + sizeof(metadata) && curr->free == 1) {
            /*FOUND A MATCH THAT CAN BE SPLIT*/
            metadata new = {curr->block_size - sizeof(metadata) - size, size, 1};
            curr->block_size = size;
            curr->free = 0;
            metadata* new_location = (metadata*)(((void*)curr)+sizeof(metadata)+curr->block_size);
            *new_location = new;
            metadata* next_location = (metadata*)(((char*)new_location) + 
                                sizeof(metadata) + new_location->block_size);
            if(((char*)next_location < ((char*)init)+4095)
                && (next_location->block_size != 0 || next_location->free <= 1)) {
                next_location->prev_block_size = new.block_size;
            }
            return ((metadata*)curr)+1;
        } else if(curr->block_size >= size && curr->free == 1) {\
            /*FOUND A MATCH THAT CANNOT BE SPLIT*/
            curr->free = 0;
            return ((metadata*)curr)+1;
        } else {
            /*CURRENT BLOCK DOESNT FIT*/
            offset = curr->block_size + sizeof(metadata);
        }
    }
    /*FOUND NO MATCH IN MEMORY, EXITING*/
    exit_malloc(4, file, line);
    return NULL;
}


/**
 * void* myfree(void* my_ptr, char* file, int line)
 * 
 * Summary:
 *     Frees the pointer my_ptr in the simulated memory.
 * 
 * Description:
 *     This function is a free() replacement--this function will check within the 
 *     simulated 4096 bytes of memory to find the my_ptr metadata block. It will then
 *     free that block of memory and merge with free blocks of memory on the left and
 *     right of the current block.
 * 
 * Parameters:
 *     my_ptr: a valid pointer to be freed--my_ptr must have been created by mymalloc().
 *     file: the current file, as given by __FILE__.
 *     line: the current line, as given by __LINE__.
 * 
 * Returns:
 *     None.
 * 
 * Throws:
 *     OUT OF BOUNDS ERROR: If my_ptr is out of bounds.
 *     INVALID POINTER ERROR: If my_ptr is invalid.
 *     FREE ERROR: If my_ptr is already freed.
 */
void myfree(void* my_ptr, char* file, int line) {
   metadata* meta = (metadata*)((char*)my_ptr-sizeof(metadata));

    /*CHECK IF OPERATION IS INVALID*/
    if(((char*)my_ptr > ((char*)init)+4095 || ((char*)my_ptr < (char*)init))) {
        return exit_malloc(1, file, line);
    } else if(meta->block_size == 0 || meta->free > 1) {
        return exit_malloc(2, file, line);
    } else if(meta->free == 1) {
        return exit_malloc(3, file, line);
    } else {
        meta->free = 1;
        metadata* prev = (metadata*)((char*)meta - meta->prev_block_size - sizeof(metadata));
        metadata* next = (metadata*)((char*)meta + meta->block_size + sizeof(metadata));
        /*CHECK LEFT AND MERGE*/
        if(prev >= init && prev->free == 1) {
            prev->block_size = prev->block_size + sizeof(metadata) + meta->block_size;
            if((char*)next < ((char*)init)+4095) {
                next->prev_block_size = prev->block_size;
            }
            meta = prev;
        }
        /* CHECK RIGHT AND MERGE */
        if((char*)next < (char*)init+4095 && next->free == 1) {
            meta->block_size = meta->block_size + sizeof(metadata) + next->block_size;
            next = (metadata*)((char*)meta + meta->block_size + sizeof(metadata));
            if((char*)next < ((char*)init)+4095) {
                next->prev_block_size = meta->block_size;
            }
       }
   }
   return;
}