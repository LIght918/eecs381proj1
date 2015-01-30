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
void print_as_int(int * data_ptr);
int compare_int(const int * data_ptr1, const int * data_ptr2);

void find_and_remove(struct Ordered_container * container, int * probe);

void demo_func(void * data_ptr, void * arg);
int demo_func2(void * data_ptr, void * arg);
int demo_func3(void * data_ptr);
int demo_func2_add(void * data_ptr, void * arg);
int demo_func3_sub(void * data_ptr);
void add_one(void * data_ptr);
void sub_one(void * data_ptr);

void insert(struct Ordered_container * container, int * insert);
void print_all(struct Ordered_container * container);

int main(void)
{		
	struct Ordered_container * container;
	int i;
	
	container = OC_create_container((int (*)(const void *, const void *))compare_int);

	srand(1);
	
	for (i = 0; i < TRIALS; i++)
	{
		int random = rand() % 12;
		switch (random)
		{
			case 0:
			{
			}
			case 1:
			{
			}
			case 2:
			{
			}
			case 3:
			{
				int *new_int = malloc(sizeof(int));
				printf("inserting\n");
				*new_int = rand() % INT_RANGE;
				insert(container, new_int);
				break;
			}
			case 4:
			{
				int search_int = rand() % INT_RANGE;
				printf("search and remove\n");
				find_and_remove(container, &search_int);
				break;
			}
			case 5:
			{
				int value = rand() % INT_RANGE;
				printf("demo_func\n");
				OC_apply_arg(container, demo_func, (void *)&value);
				break;
			}
			case 6:
			{
				int value = rand() % INT_RANGE;
				printf("demo_func2\n");
				OC_apply_if_arg(container, demo_func2, (void *)&value);
				break;
			}
			case 7:
			{
				printf("demo_func3\n");
				OC_apply_if(container, demo_func3);
				break;
			}
			case 8:
			{
				printf("add_one\n");
				OC_apply(container, add_one);
				break;
			}
			case 9:
			{
				printf("sub_one\n");
				OC_apply(container, sub_one);
				break;
			}
			case 10:
			{
				int value = rand() % INT_RANGE;
				printf("demo_func2_add\n");
				OC_apply_if_arg(container, demo_func2_add, (void *)&value);
				break;
			}
			case 11:
			{
				printf("demo_func3_sub\n");
				OC_apply_if(container, demo_func3_sub);
				break;
			}
			default:
			{
				break;
			}
		}
		printf("\n");
		printf("print\n");
		print_all(container);
		printf("\n");
	}

	print_all(container);

	OC_clear(container);

	print_all(container);
	
	OC_destroy_container(container);
	/* using the pointer "container" is undefined at this point */
	
	printf("Done\n");
	return 0;
}

void print_as_int(int * data_ptr)
{
	printf("%d\n", *data_ptr);
}

int compare_int(const int * data_ptr1, const int * data_ptr2)
{
	return *data_ptr1 - *data_ptr2;
}

void find_and_remove(struct Ordered_container * container, int * probe)
{
	void * found_item;
	printf("search for %d:\n", *probe);
	found_item = OC_find_item(container, probe);
	if(found_item) {
		printf("found %d\n", *((int *)OC_get_data_ptr(found_item)));
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
	printf("inserting %d\n", *insert);
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
	if ((*int_ptr + *((int *)data_ptr)) % 2)
		printf("%d + %d = odd\n", *((int *)data_ptr), *int_ptr);
	else
		printf("%d + %d = even\n", *((int *)data_ptr), *int_ptr);
}

int demo_func2(void * data_ptr, void * arg)
{
	int * int_ptr = (int *)arg;
	if (*((int *)data_ptr) > *((int *)arg))
	{
		printf("%d > %d\n", *((int *)data_ptr), *int_ptr);
		return *((int *)data_ptr);
	}
	else
	{
		printf("%d <= %d\n", *((int *)data_ptr), *int_ptr);
		return 0;
	}
}

int demo_func3(void * data_ptr)
{
	if (*((int *)data_ptr) % 2)
	{
		printf("%d = odd\n", *((int *)data_ptr));
		return *((int *)data_ptr);
	}
	else
	{
		printf("%d = even\n", *((int *)data_ptr));
		return 0;
	}
}

int demo_func2_add(void * data_ptr, void * arg)
{
	add_one(data_ptr);
	return demo_func2(data_ptr, arg);
}

int demo_func3_sub(void * data_ptr)
{
	sub_one(data_ptr);
	return demo_func3(data_ptr);
}

void add_one(void * data_ptr)
{
	*((int*)data_ptr) = (*((int*)data_ptr) + 1) % INT_RANGE;
}

void sub_one(void * data_ptr)
{
	*((int*)data_ptr) = (*((int*)data_ptr) - 1) % INT_RANGE;
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

