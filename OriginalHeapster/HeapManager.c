#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Block {
	int block_size;				//# of bytes in the data section
	struct Block *next_block;   //a pointer to the next free block of mem
};								//in C, you have to use "struct Block" as the type

const int overhead_size = sizeof(struct Block); //8
const int ptr_size = sizeof(void*); //4
struct Block *free_head;

void my_initialize_heap(int size) {
	//create free_head -> [size|next_ptr|#############]
	free_head = (struct Block *) malloc(size);
	free_head->block_size = size - overhead_size;
	free_head->next_block = NULL;

	printf("heap address: %d\n", free_head);
}

void* my_alloc(int size) {
	//size must be a multiple of ptr_size
	size += size % ptr_size;
	//printf("size is %d\n", size);
	//walk the free list starting at free_head
	int cont = 1;

	struct Block *prev_block = free_head;
	struct Block *current_block = free_head;
	while (cont) {
		if (current_block->block_size >= size) {
			//printf("block size: %d\n", current_block->block_size);
			cont = 0;
		}
		else if (current_block->next_block != NULL) {
			prev_block = current_block; //update prev_block
			current_block = current_block->next_block;
		}
		else {
			return 0;
		}
	}

	//decide whether or not to split the data block
	bool canSplit = current_block->block_size >= size + overhead_size + ptr_size;
	bool isHead = current_block == free_head;
	printf("canSplit = %d\n", canSplit);
	printf("isHead = %d\n", isHead);
	if (canSplit && isHead) {
		//instantiate the new free block within the current block's data portion
		//current block will be used for allocating data
		struct Block *new_block = (struct Block *)((char *)current_block + overhead_size + size);
		new_block->block_size = current_block->block_size - (size + overhead_size);
		new_block->next_block = current_block->next_block;

		//set free_head to the new block created
		free_head = new_block;
		printf("free head size after splitting: %d\n", free_head->block_size);

		current_block->block_size = size; //update size
	}
	else if (canSplit && !isHead) {
		//instantiate the new free block within the current block's data portion
		//current block will be used for allocating data
		struct Block *new_block = (struct Block *)(char *)current_block + overhead_size + size;
		new_block->block_size = current_block->block_size - (size + overhead_size);
		new_block->next_block = current_block->next_block;

		//update pointer of prev_block to point to following block
		prev_block->next_block = current_block->next_block;
	}
	else if (!canSplit && isHead) {
		free_head = current_block->next_block;
		//printf("update free_head: %d\n", free_head->block_size);
	}
	else {
		prev_block->next_block = current_block->next_block;
	}
	//printf("allocated block size: %d\n", current_block->block_size);
	return current_block + 1; //return a pointer to the data portion
}

void my_free(void *data) {
	struct Block *free_block = (struct Block *)data - 1;
	//printf("free head size: %d\n", free_head->block_size);
	//printf("newly freed block size: %d\n", free_block->block_size);
	free_block->next_block = free_head;
	free_head = free_block;
	printf("new free head size after update: %d\n", free_head->block_size);
}

int main() {
	my_initialize_heap(6400);

	int *a = my_alloc(sizeof(int)); //allocate an int
	printf("a address: %d\nfreeing a now...\n", a);

	my_free(a);
	int *b = my_alloc(sizeof(int)); //allocate another int
	printf("b address: %d\n", b);
	system("PAUSE");
	return 0;
}