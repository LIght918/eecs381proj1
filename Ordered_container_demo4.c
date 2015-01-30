/*
This contains a demo of the Ordered_container module; the behavior is the same regardless of whether
the container or array implementation is used. 

This demo uses a container of pointers to string literals; more commonly they
would be dynamically allocated.

Use a simple program like this as a "test harness" to systematically test your Ordered_container
functions, starting with the simplest and most basic. Be sure that you test them all!
*/

#include <stdio.h>
#include <string.h>
#include "Ordered_container.h"
#include <stdlib.h>

#define TRIALS 1000
#define INT_RANGE 350

/* function prototypes */
void print_as_int(char * data_ptr);
int compare_int(const char * data_ptr1, const char * data_ptr2);

void find_and_remove(struct Ordered_container * container, char * probe);
void demo_func(void * data_ptr, void * arg);
void insert(struct Ordered_container * container, int * insert);
void print_all(struct Ordered_container * container);

int main(void)
{		
	struct Ordered_container * container;
	int i;
	
	container = OC_create_container((int (*)(const void *, const void *))compare_int);

	srand(0);
	
	for (i = 0; i < TRIALS; i++)
	{
		int random = rand() % 4;
		switch (random)
		{
			case 0:
			{
			}
			case 1:
			{
				int *new_int = malloc(sizeof(int));
				*new_int = rand() % INT_RANGE;
				insert(container, new_int);
				break;
			}
			case 2:
			{
				int search_int = rand() % INT_RANGE;
				find_and_remove(container, &search_int);
				break;
			}
			case 3:
			{
				int value = rand() % INT_RANGE;
				OC_apply_arg(container, demo_func, (void *)&value);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	/* demo another apply function - note function pointer cast is not needed for this one */
	{
		int odd_or_even_value = 42;
		OC_apply_arg(container, demo_func, (void *)&odd_or_even_value);
		odd_or_even_value = 3;
		OC_apply_arg(container, demo_func, (void *)&odd_or_even_value);
	}

	OC_clear(container);

	print_all(container);
	
	OC_destroy_container(container);
	/* using the pointer "container" is undefined at this point */
	
	printf("Done\n");
	return 0;
}

void print_as_int(int * data_ptr)
{
	printf("%d\n", data_ptr);
}

int compare_int(const int * data_ptr1, const int * data_ptr2)
{
	return strcmp(data_ptr1, data_ptr2);
}

void find_and_remove(struct Ordered_container * container, int * probe)
{
	void * found_item;
	printf("\nsearch for %d:\n", probe);
	found_item = OC_find_item(container, probe);
	if(found_item) {
		printf("found item points to %d\n", (int *)OC_get_data_ptr(found_item));
		OC_delete_item(container, found_item);
		printf("item removed\n");
	}
	else
	{
		printf("probed item not found\n");
	}
}

void insert(struct Ordered_container * container, int * insert)
{
	printf("\ninserting %d\n", *insert);
	OC_insert(container, insert);
}

void print_all(struct Ordered_container * container)
{
	printf("size is %d\n", OC_get_size(container));
	OC_apply(container, (void(*)(void *))print_as_int);
}

void demo_func(void * data_ptr, void * arg)
{
	int * int_ptr = (int *)arg;
	if((*int_ptr + *data_ptr) % 2) /* is the arg even or odd? */
		printf("I like this item: %s\n", (int *)data_ptr);
	else
		printf("I hate this item: %s\n", (int *)data_ptr);
}


/* output
size is 0

inserting s3
size is 1
s3

inserting s1
size is 2
s1
s3

inserting s4
size is 3
s1
s3
s4

inserting s2
size is 4
s1
s2
s3
s4

search for s3:
found item points to s3
item removed
size is 3
s1
s2
s4

search for s99:
probed item not found
size is 3
s1
s2
s4
I hate this item: s1
I hate this item: s2
I hate this item: s4
I like this item: s1
I like this item: s2
I like this item: s4
size is 0
Done
*/

