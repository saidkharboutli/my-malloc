/**
 * File:        memgrind.c
 * Author:      Mohammad S Kharboutli
 * Date:        25 OCT 2020
 * Course:      CS214: Systems Programming
 * University:  Rutgers--New Brunswick
 * 
 * Summary of File:
 *     This C file is a thorough performance and functionality
 *     tester of our custom malloc() and free() implementations. The
 *     tests range from allocating singular bytes and freeing each byte
 *     to cycling through error testing and finally, a look into the
 *     performance difference between splitting blocks and not splitting.
 */

#include "mymalloc.h"
#include <sys/time.h>

int main(int argc, char** argv) {
    float meanA = 0;
    float meanB = 0;
    float meanC = 0;
    float meanD = 0;
    float meanE = 0;

    struct timeval start, end;

    for(int c = 0; c < 50; c++) {
        /*TEST CASE A*/
        gettimeofday(&start, NULL);
        for(int i = 0; i < 120; i++) {
            free(malloc(1));
        }
        gettimeofday(&end, NULL);
        meanA += (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec)*1000000);


        /*TEST CASE B*/
        void* ptr_arrB[120];
        gettimeofday(&start, NULL);
        for(int i = 0; i < 240; i++) {
            if(i < 120) {
                ptr_arrB[i] = malloc(1);
            } else {
                free(ptr_arrB[239 - i]);
            }
        }
        gettimeofday(&end, NULL);
        meanB += (end.tv_usec - start.tv_usec+ (end.tv_sec - start.tv_sec)*1000000);


        /*TEST CASE C*/
        srand(123422134);
        int total_alloc = 0;
        int total_free = 0;
        int array_index = 0;
        void* ptr_arrC[120];
        gettimeofday(&start, NULL);
        for(int i = 0; i < 240; i++) {
            int my_rand = rand() % 2;
            if(my_rand && array_index && total_free < 120) {
                array_index--;
                free(ptr_arrC[array_index]);
                total_free++;
            } else if(!my_rand && total_alloc < 120) {
                ptr_arrC[array_index] = malloc(1);
                array_index++;
                total_alloc++;
            } else if(my_rand) {
                ptr_arrC[array_index] = malloc(1);
                array_index++;
                total_alloc++;
            } else if(!my_rand) {
                array_index--;
                free(ptr_arrC[array_index]);
                total_free++;
            }
        }
        gettimeofday(&end, NULL);
        meanC += (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec)*1000000);


        /**
         * D WILL DEMONSTRATE OUR ERROR-HANDLING; IN ORDER TO DO THAT
         * WE MUST ENSURE THAT THE MANY, MANY FRAGMENTS OF MEMORY WE
         * HAVE WRITTEN ARE WRITTEN OVER SO THAT OUR ERROR HANDLING
         * IS MORE ACCURATE. REGARDLESS OF WHETHER OR NOT WE CLEAR
         * THE BLOCK OF MEMORY, THE CODE WILL NOT RETURN A POINTER
         * IF THERE IS ANY ISSUE, AS DISCUSSED IN THE ASSIGNEMNT
         * WRITE UP. HOWEVER WE WILL COMMONLY SEE THAT OUR 1B BLOCKS
         */ 
        void* clear_mem = malloc(4090);
        for(int i = 0; i < 4095; i++) {
            *(((char*)clear_mem)+i) = 0;
        }
        free(clear_mem);


        /*TEST CASE D*/
        printf("\n[%d] BEGIN DELIBERATE ERROR TESTING:\n----------------------------\n", c+1);
        void* out_of_bounds = (void*) 0x123456;
        void* my_ptr = malloc(120);
        gettimeofday(&start, NULL);
        free(out_of_bounds);                        //ERROR CODE 1
        free(my_ptr+50);                            //ERROR CODE 2
        free(my_ptr);
        free(my_ptr);                               //ERROR CODE 3
        malloc(4096);                               //ERROR CODE 4
        malloc(-120);                               //ERROR CODE 5
        gettimeofday(&end, NULL);
        meanD += (end.tv_usec - start.tv_usec);
        printf("----------------------------\n[%d] END DELIBERATE ERROR TESTING \n", c+1);


        /*TEST CASE E*/
        gettimeofday(&start, NULL);
        for(int i = 0; i < 30; i++) {
            void* left = malloc(500);
            void* to_split = malloc(500);
            void* right = malloc(500);
            free(to_split);
            void* split_ptr = malloc(400);
            free(left);
            free(right);
            free(split_ptr);
        }
        gettimeofday(&end, NULL);
        float time_reg = ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec)*1000000);
        gettimeofday(&start, NULL);
        for(int i = 0; i < 30; i++) {
            void* left = malloc(500);
            void* to_split = malloc(500);
            void* right = malloc(500);
            free(to_split);
            void* split_ptr = malloc(496);
            free(left);
            free(right);
            free(split_ptr);
        }
        gettimeofday(&end, NULL);
        meanE += time_reg - (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec)*1000000);
    }

    printf("\nAVG TIME A: %f uS", meanA / 50.0);
    printf("\nAVG TIME B: %f uS", meanB / 50.0);
    printf("\nAVG TIME C: %f uS", meanC / 50.0);
    printf("\nAVG TIME D: %f uS", meanD / 50.0);
    printf("\nAVG TIME E: %f uS", meanE / 50.0);
    printf("\n----------------------------");
    printf("\nAVG TIME TOTAL: %f uS\n", (meanA + meanB + meanC + meanD + meanE) / 50.0);
    return 0;
}