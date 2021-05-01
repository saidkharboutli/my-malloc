CC=gcc
CFLAGS=-I.
DEPS = mymalloc.h
OBJ = mymalloc.o memgrind.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

memgrind: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f memgrind *.o