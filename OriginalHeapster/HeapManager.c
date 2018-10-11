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

	//printf("heap address: %d\n", free_head);
}

void* my_alloc(int size) {
	//size must be a multiple of ptr_size
	if (size % ptr_size != 0) {
		size += ptr_size - (size % ptr_size);
	}
	printf("size is %d\n", size);
	//walk the free list starting at free_head
	int cont = 1;

	struct Block *prev_block = free_head;
	struct Block *current_block = free_head;
	while (cont) {
		printf("current_block address: %d\n", current_block);
		printf("current_block size: %d\n", current_block->block_size);
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
	//printf("canSplit = %d\n", canSplit);
	//printf("isHead = %d\n", isHead);
	if (canSplit && isHead) {
		//instantiate the new free block within the current block's data portion
		//current block will be used for allocating data
		struct Block *new_block = (struct Block *)((char *)current_block + overhead_size + size);
		new_block->block_size = current_block->block_size - (size + overhead_size);
		new_block->next_block = current_block->next_block;

		//set free_head to the new block created
		free_head = new_block;
		printf("free head address: %d\n", free_head);

		current_block->block_size = size; //update size
	}
	else if (canSplit && !isHead) {
		//instantiate the new free block within the current block's data portion
		//current block will be used for allocating data
		struct Block *new_block = (struct Block *)(char *)current_block + overhead_size + size;
		new_block->block_size = current_block->block_size - (size + overhead_size);
		new_block->next_block = current_block->next_block;

		//update pointer of prev_block to point to following block
		prev_block->next_block = new_block;
	}
	else if (!canSplit && isHead) {
		free_head = current_block->next_block;
		printf("free head address: %d\n", free_head);
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
	printf("free head address: %d\n", free_head);
}

int main() {
	my_initialize_heap(8000);

	//test 1: allocating 1 int, freeing it, then allocating another
	//both should have the same address
	printf("Test 1\n");
	int *a = my_alloc(sizeof(int)); //allocate an int
	printf("int a address: %d\nfreeing a now...\n", a);
	my_free(a);
	int *b = my_alloc(sizeof(int)); //allocate another int
	printf("int b address: %d\n", b);
	system("PAUSE");

	//test 2
	printf("\nTest 2\n");
	int *c = my_alloc(sizeof(int));
	int *d = my_alloc(sizeof(int));
	printf("int c address: %d\n", c);
	printf("int d address: %d\n", d);
	system("PAUSE");

	//test 3
	printf("\nTest 3\n");
	int *e = my_alloc(sizeof(int));
	int *f = my_alloc(sizeof(int));
	int *g = my_alloc(sizeof(int));
	printf("int e address: %d\n", e);
	printf("int f address: %d\n", f);
	printf("int g address: %d\nnow freeing int f\n", g);
	my_free(f);
	printf("allocating an array of two double values...\n");
	double *arr = my_alloc(2 * sizeof(double));
	printf("double array address: %d\n", arr);
	int *h = my_alloc(sizeof(int));
	printf("int h address: %d\n", h);
	printf("Compare int h address to int f address\n");
	system("PAUSE");

	//test 4
	printf("\nTest 4\n");
	char *char_data = my_alloc(sizeof(char));
	int *i = my_alloc(sizeof(int));
	printf("char_data address: %d\n", char_data);
	printf("int i address: %d\n", i);
	system("PAUSE");

	//test 5
	printf("\nTest 5\n");
	int *int_arr = my_alloc(80 * sizeof(int));
	int *j = my_alloc(sizeof(int));
	printf("int_arr address: %d\n", int_arr);
	printf("int j address: %d\nfreeing int_arr...\n", j);
	my_free(int_arr);
	printf("int j address: %d\n", j);
	system("PAUSE");

	return 0;
}