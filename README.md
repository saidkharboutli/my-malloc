# mymalloc(3), myfree(3) and memgrind

## Metadata Design
As discussed in lecture, and as we know from the field of computer science, when
designing a solution to a problem the main performance considerations will always be space and
time complexity and the compromise between the two concepts. For a function my malloc(), this
is especially important, because malloc() depends on segmenting memory properly, and to do
that it will always require some metadata to mark used memory and an algorithm to check and
segment off each piece of memory.
Within the scope of my project, I made choices that took preference to space complexity,
even though that requires more calculations and compromises readability and the size of the
program. Initially, I started out with a struct with 3 parts:

struct metadata {
short block_size;
unsigned char free;
struct metadata* next;
};

However I found that, even though I thought this should only be 12 bytes, sizeof() gave
me 16 bytes. This, I found out, was due to memory packing/padding which aligns byte data for
the processor, increasing performance. For a 4096 byte block of memory, 16 bytes is a
substantial amount of memory to store nothing but unusable metadata. Thus, I decided to rethink
how I was going to flag my segments. I realized two things: first, I would be able to easily find
the next block with a simple addition of pointers using the already-existing block_size . That
got rid of the need for a next pointer. Then I realized that when we merge we actually need a
way to go to the previous block as well, since we will need to check the previous block when
merging. Thus, I decided to use another short ( short because it is 2 bytes long and that is all
we need to store a max value of 4096; 1 byte is not enough, on the other hand) to help in finding
the previous block. This led to:

struct metadata {
short block_size;
short prev_block_size;
unsigned char free;
};

This configuration complicated some of the code where I could have just used pointers to
move from one block to another, but it saved 10 bytes . The sizeof() this struct on Windows,
Debian/ParrotOS, and Ubuntu was 6 bytes. 2 shorts at 2 bytes each, and a single 1 byte
unsigned char . The extra byte is a result of memory packing/padding as discussed before. I
decided that it was not worth the hassle of placing char and short data directly into the block
of memory without the struct wrapper to save 1 byte.