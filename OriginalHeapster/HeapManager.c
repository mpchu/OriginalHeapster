//CECS 424 - Original Heapster
//Author: Matthew Chu
//10/11/2018
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Block {
	int block_size;				//# of bytes in the data section
	struct Block *next_block;   //a pointer to the next free block of mem
};								//in C, you have to use "struct Block" as the type

const int overhead_size = sizeof(struct Block);
const int ptr_size = sizeof(void*);
struct Block *free_head;

void my_initialize_heap(int size) {
	//create free_head -> [size|next_ptr|#############]
	free_head = (struct Block *) malloc(size);
	free_head->block_size = size - overhead_size;
	free_head->next_block = NULL;
}

void* my_alloc(int size) {
	//size must be a multiple of ptr_size
	if (size % ptr_size != 0) {
		size += ptr_size - (size % ptr_size);
	}

	//walk the free list starting at free_head
	int cont = 1;

	struct Block *prev_block = free_head;
	struct Block *current_block = free_head;
	while (cont) {
		if (current_block->block_size >= size) {
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
	if (canSplit && isHead) {
		//instantiate the new free block within the current block's data portion
		//current block will be used for allocating data
		struct Block *new_block = (struct Block *)((char *)current_block + overhead_size + size);
		new_block->block_size = current_block->block_size - (size + overhead_size);
		new_block->next_block = current_block->next_block;

		//set free_head to the new block created
		free_head = new_block;

		current_block->block_size = size; //update size
	}
	else if (canSplit && !isHead) {
		//instantiate the new free block within the current block's data portion
		//current block will be used for allocating data
		struct Block *new_block = (struct Block *)(char *)current_block + overhead_size + size;
		new_block->block_size = current_block->block_size - (size + overhead_size);
		new_block->next_block = current_block->next_block;

		current_block->block_size = size; //update size

		//update pointer of prev_block to point to following block
		prev_block->next_block = new_block;
	}
	else if (!canSplit && isHead) {
		free_head = current_block->next_block;
	}
	else {
		prev_block->next_block = current_block->next_block;
	}
	return current_block + 1; //return a pointer to the data portion
}

void my_free(void *data) {
	struct Block *free_block = (struct Block *)data - 1;
	free_block->next_block = free_head;
	free_head = free_block;
}

//test 1
void test1() {
	printf("\nTest 1\n");
	my_initialize_heap(1000);
	int *a = my_alloc(sizeof(int)); //allocate an int
	printf("int a address: %d\nfreeing int a...\n", a);
	my_free(a);
	int *b = my_alloc(sizeof(int)); //allocate another int
	printf("int b address: %d\n", b);
}

	//test 2
void test2() {
	printf("\nTest 2\n");
	my_initialize_heap(1000);
	int *c = my_alloc(sizeof(int));
	printf("int c address: %d\n", c);
	int *d = my_alloc(sizeof(int));
	printf("int d address: %d\n", d);
}

//test 3
void test3() {
	printf("\nTest 3\n");
	my_initialize_heap(1000);
	int *e = my_alloc(sizeof(int));
	printf("int e address: %d\n", e);
	int *f = my_alloc(sizeof(int));
	printf("int f address: %d\n", f);
	int *g = my_alloc(sizeof(int));
	printf("int g address: %d\nfreeing int f...\n", g);
	my_free(f);
	printf("allocating an array of two double values...\n");
	double *arr = my_alloc(2 * sizeof(double));
	printf("double array address: %d\n", arr);
	int *h = my_alloc(sizeof(int));
	printf("int h address: %d\n", h);
	printf("Are the addresses of int h and int f the same? (Hopefully yes)\n");
}

//test 4
void test4() {
	printf("\nTest 4\n");
	my_initialize_heap(1000);
	char *char_data = my_alloc(sizeof(char));
	int *i = my_alloc(sizeof(int));
	printf("char_data address: %d\n", char_data);
	printf("int i address: %d\n", i);
}

//test 5
void test5() {
	printf("\nTest 5\n");
	my_initialize_heap(1000);
	int *int_arr = my_alloc(80 * sizeof(int));
	int *j = my_alloc(sizeof(int));
	*j = 7;
	printf("int_arr address: %d\n", int_arr);
	
	printf("int j address: %d\nint j value: %d\nfreeing int_arr...\n", j, *j);
	my_free(int_arr);
	printf("int j address after freeing int_arr: %d\nint j value after freeing int_arr: %d\n", j, *j);
}

int main() {
	printf("Overhead size is: %d\n", overhead_size);
	printf("Pointer size is: %d\n", ptr_size);
	int input = -1;
	while (input != 0) {
		printf("Which case would you like to test? (0 to exit)\n");
		scanf_s("%d", &input);

		switch (input)
		{
			case -1:
				printf("Exiting program!\n");
				break;
			case 1:
				test1();
				printf("\n");
				break;
			case 2:
				test2();
				printf("\n");
				break;
			case 3:
				test3();
				printf("\n");
				break;
			case 4:
				test4();
				printf("\n");
				break;
			case 5:
				test5();
				printf("\n");
				break;
			default:
				printf("Please input a number between 1 and 5 or input 0 to exit\n");
				break;
		}
	}

	return 0;
}

//Test Case outputs
// Test 1
// int a address : 493552
// freeing int a...
// int b address : 493552

// Test 2
// int c address : 494600
// int d address : 494612

// Test 3
// int e address : 508016
// int f address : 508028
// int g address : 508040
// freeing int f...
// allocating an array of two double values...
// double array address : 508052
// int h address : 508028
// Are the addresses of int h and int f the same? (Hopefully yes)

// Test 4
// char_data address : 509064
// int i address : 509076

// Test 5
// int_arr address : 510112
// int j address : 510440
// int j value : 7
// freeing int_arr...
// int j address after freeing int_arr : 510440
// int j value after freeing int_arr : 7