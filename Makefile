CC = gcc
CFLAGS = -fsanitize=address -std=c99

all: memgrind

mymalloc: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -o my_malloc mymalloc.c

memgrind: mymalloc.c memgrind.c memgrind.h
	$(CC) $(CFLAGS) -c mymalloc.c
	$(CC) $(CFLAGS) -o memgrind memgrind.c mymalloc.o

clean:
	rm -rf *.o memgrind
