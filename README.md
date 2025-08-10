### FUNCTIONALITIES ###

# PADDING

Blocks are always rounded up by 8 bytes (if block is 33 Bytes, it is rounded to 40 Bytes) <- Improves the general efficiency of the CPU

# MALLOC

1. Searches for free fragmented blocks, if their combined size is enough for the heap request, they are coalesced 
2. Searches for a free block
3. Slices a free block into 2 if it's more than two times bigger compared to the requested sice
4. Coalesces adjacent free blocks if their combined size is enough for the heap request
5. If free block is not found, performs a syscall that moves the program break sbrk() and creates a new block
6. Returns void* right after the block's metadata

# REALLOC <- Complete reallocation of data is last possible measure (currently realloc() only increases the size of the block, doesn't shrink it)

1. If block is last, performs a syscall that moves the program break sbrk(), and the metadata is modified
2. If adjacent block is free and the combined size of both is enough for the heap request, both blocks are coalesced
3. If there is a free block with enough size, the current block is freed and data is moved to the new block
4. If size of the block is more than twice as bigger compared to the requested memory + already allocated one, the free block is sliced in two parts
4. If no suitable block is found, the current block is freed, a new one is created and the data is moved from the old block to the new 
5. Returns void* right after the block's metadata   

# FREE

1. Checks if adjacent to the chosen for freeing block is free as well
2. If it's free, it coalesces them into 1
3. If it's not, it just frees the chosen block
