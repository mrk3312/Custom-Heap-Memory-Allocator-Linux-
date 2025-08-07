#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H

#include <stddef.h>
#include <unistd.h>
#include <string.h>

void *my_malloc (size_t size); 
void *my_realloc (void *ptr, size_t size);
int my_free (void *ptr);

#endif