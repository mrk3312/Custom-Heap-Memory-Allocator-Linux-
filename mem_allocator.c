#include "mem_allocator.h"

#define ALIGN8(x) (((x) + 7) & ~7)

typedef struct s_block *t_block;

typedef struct s_block
{
    int size;
    t_block next;
    int free;
} s_block;

t_block global_base = NULL;

t_block find_last_block(void)
{
    if (!global_base)
        return NULL;
    

    t_block current = global_base;
    t_block last_block = global_base;
    int counter = 0;
    while (current)
    {
        counter++;
        last_block = current;
        current = current->next;
    }
    return last_block; 
}

t_block find_free_block(size_t size)
{
    t_block current = global_base;

    while (current && !(current->free && current->size >= size))
    {
        current = current->next;
    }
    return current;
}

t_block find_free_blocks_fragmented(size_t size)
{
    if (!global_base)
        return NULL;
        
    t_block last = global_base;
    t_block current = global_base->next;
    
    if (!current)
    {
        return NULL;
    }

    while (current)
    {
        if ((last->free && current->free && last->size + current->size >= size))
        {
            return last;
        }

        last = current;
        current = current->next;
    }
    return NULL;
}

t_block merge_blocks(t_block last)
{   
    t_block current = last->next;
    last->size += current->size + sizeof(s_block);
    last->next = current->next;
    current = NULL;
    return last;
}

t_block request_block(t_block last_block, size_t size)
{
    t_block block = (t_block)sbrk(sizeof(s_block) + size);
    block->size = size;
    block->free = 0;
    block->next = NULL;

    if (!global_base)
    {
        global_base = block;
        return block;
    }

    last_block->next = block;
    return block;
}

void *my_malloc(size_t size)
{
    size = ALIGN8(size);
    t_block block_fragmented = find_free_blocks_fragmented(size);
    t_block block;
    t_block last_block = find_last_block();

    if (block_fragmented)
    {
        block = merge_blocks(block_fragmented);
        return (void*)(block + 1);
    }
    
    block = find_free_block(size);

    if (block)
    {
        if (block->size > size * 2)
        {
            size_t total_size = block->size + sizeof(s_block);
            __uint8_t *ptr = (__uint8_t *)(block) + total_size;

            t_block n_block = (t_block)ptr;
            n_block->size = size;
            n_block->next = block->next;
            n_block->free = 0;

            block->size -= (size + sizeof(s_block));
            block->next = n_block;
            block->free = 1;
            return (void*)(n_block + 1);
        }
        return (void*)(block + 1);
    }

    block = request_block(last_block, size);
    return (void*)(block + 1);
}

int my_free(void *ptr)
{
    if (!ptr)
        return -1;

    t_block block = (t_block)ptr - 1;
    t_block n_block;
    if (block->next)
    {
        n_block = block->next;
        if (n_block->free)
        {
            block->size += n_block->size + sizeof(s_block);
            if (n_block->next)
                block->next = n_block->next;
            else
                block->next = NULL;
            n_block = NULL;

        }
    }
    block->free = 1;
    return 1;
}

t_block resize_block(t_block block, size_t size)
{
    sbrk(size);
    block->size += size;
    return block;
}

void *my_realloc(void *ptr, size_t size)
{
    if (!ptr)
        return NULL;

    size = ALIGN8(size);
    t_block block = (t_block)ptr - 1;
    t_block last_block = find_last_block();

    if (!block->next)
    {
        block = resize_block(block, size);
        return (void*)(block + 1);
    }

    if (block->next->free && (block->next->size + sizeof(s_block) >= size))
    {
        block = merge_blocks(block);
        return (void *)(block + 1);
    }
    t_block block_fragmented = find_free_blocks_fragmented(size);
    if (block_fragmented)
    {       
        if (block_fragmented->size + block_fragmented->next->size + sizeof(s_block) >= block->size + size)
        {
            block_fragmented = merge_blocks(block_fragmented);
            void *second_ptr = (void *)(block_fragmented + 1);
            memcpy(second_ptr, ptr, block->size);
            block_fragmented->free = 0;
            block->free = 1;
            return (void*)(block_fragmented + 1);
        }
    }
    
    t_block n_block = request_block(last_block, size);
    void *second_ptr = (void *) (n_block + 1);

    memcpy(second_ptr, ptr, block->size);
    block->free = 1;
    n_block->free = 0;
    return (void*)(n_block + 1);
}