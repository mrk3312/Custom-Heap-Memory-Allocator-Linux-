CFLAGS := -Wall -fPIC -O0

all: libmem_allocator.so main

mem_allocator.o: mem_allocator.c mem_allocator.h
	gcc $(CFLAGS) -c mem_allocator.c -o mem_allocator.o

libmem_allocator.so: mem_allocator.o
	gcc -shared -o libmem_allocator.so mem_allocator.o

main: main.c libmem_allocator.so
	gcc main.c -L. -lmem_allocator -o main

clean:
	rm -f *.o *.so main

