#ifndef ORDERED_CONTAINER_ARRAY
#define ORDERED_CONTAINER_ARRAY

#include "Ordered_container.h"
#include "Utility.h"
#include "p1_globals.h"
#include <stdlib.h>

#define SIZE_FACTOR 2
#define ALLOCATION_INCREASE 1
#define INITIAL_ALLOCATION 3

/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container {
	OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
	void** array;			/* pointer to array of pointers to void */
	int allocation;			/* current size of array */
	int size;				/* number of items  currently in the array */
};

/* Enum for OC_apply functions */
typedef enum { APPLY, APPLY_IF, APPLY_ARG, APPLY_ARG_IF, APPLY_INTERNAL
} apply_enum;

/* Struct returned by OC_binary_search */
struct Search_Result {
	int found; /* whether or not the value was found */
	int index; /* the index of the element if found, or the element directly after */
};

/* These global variables are used to monitor the memory usage of the Ordered_container */
int g_Container_count = 0;				/* number of Ordered_containers currently allocated */
int g_Container_items_in_use = 0;		/* number of Ordered_container items currently in use */
int g_Container_items_allocated = 0;	/* number of Ordered_container items currently allocated */

/*
Private helper functions declarations
*/

/* Searches for the specified element using the given comparison function */
static struct Search_Result OC_binary_search(const struct Ordered_container* c_ptr, const void* data_ptr, OC_comp_fp_t comp_fun);

/* Initialize the container to default values */
static void OC_initialize_container(struct Ordered_container* c_ptr);

/* Reallocate array */
static void OC_reallocate_array(struct Ordered_container* c_ptr);

/* Grabs the data ptr from the item directly preceding this item*/
static int OC_take_value_from_left(void* item_ptr);

/* Grabs the data ptr from the item directly after this item*/
static int OC_take_value_from_right(void* item_ptr);

/* Type of function used to pass function pointers around OC_apply functions */
typedef void(*OC_apply_template_fp_t) (void);

/* Type of function used by OC_apply for APPLY_INTERNAL */
typedef int(*OC_apply_internal_fp_t) (void* item_ptr);

/* Helper function for OC_apply functions
Performed on range [start, end), in order depending on reverse */
static int OC_apply_helper(const struct Ordered_container* c_ptr, OC_apply_template_fp_t afp, void* arg_ptr, apply_enum apply_func, int start, int end, int reverse);

/* Simplified call to OC_apply_helper */
static int OC_apply_helper_simple(const struct Ordered_container* c_ptr, OC_apply_template_fp_t afp, void* arg_ptr, apply_enum apply_func);

/*
Functions for the entire container.
*/

/* Create an empty container using the supplied comparison function, and return the pointer to it. */
struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
	struct Ordered_container *c_ptr = (struct Ordered_container*)malloc(sizeof(struct Ordered_container));
	c_ptr->comp_fun = f_ptr;
	OC_initialize_container(c_ptr);
	g_Container_count++;
	return c_ptr;
}

/* Destroy the container and its items; caller is responsible for
deleting all pointed-to data before calling this function.
After this call, the container pointer value must not be used again. */
void OC_destroy_container(struct Ordered_container* c_ptr)
{
	g_Container_items_in_use -= c_ptr->size;
	g_Container_items_allocated -= c_ptr->allocation;
	free(c_ptr->array);
	free(c_ptr);
	g_Container_count--;
}

/* Delete all the items in the container and initialize it.
Caller is responsible for deleting any pointed-to data first. */
void OC_clear(struct Ordered_container* c_ptr)
{
	g_Container_items_in_use -= c_ptr->size;
	g_Container_items_allocated -= c_ptr->allocation;
	free(c_ptr->array);
	OC_initialize_container(c_ptr);
}

/* Return the number of items currently stored in the container */
int OC_get_size(const struct Ordered_container* c_ptr)
{
	return c_ptr->size;
}

/* Return non-zero (true) if the container is empty, zero (false) if the container is non-empty */
int OC_empty(const struct Ordered_container* c_ptr)
{
	return c_ptr->size == 0;
}

/*
Functions for working with individual items in the container.
*/

/* Get the data object pointer from an item. */
void* OC_get_data_ptr(const void* item_ptr)
{
	return *((void**)item_ptr);
}

/* Delete the specified item.
Caller is responsible for any deletion of the data pointed to by the item. */
void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
	OC_apply_helper(c_ptr, (OC_apply_template_fp_t)OC_take_value_from_right, NULL, APPLY_INTERNAL, (void **)item_ptr - c_ptr->array, c_ptr->size - 1, 0);
	c_ptr->size--;
	g_Container_items_in_use--;
}

/*
Functions that search and insert into the container using the supplied comparison function.
*/

/* Create a new item for the specified data pointer and put it in the container in order.
If there is already an item in the container that compares equal to new item according to
the comparison function, the order of the new item relative to the existing item is not specified.
This function will not modify the pointed-to data. */
void OC_insert(struct Ordered_container* c_ptr, const void* data_ptr)
{
	int index_for_insert = 0;
	if (c_ptr->size > 0)
	{
			struct Search_Result result = OC_binary_search(c_ptr, data_ptr, c_ptr->comp_fun);
			printf("result index = %d\n", result.index);
			if (c_ptr->size == c_ptr->allocation)
			{
				printf("need to allocate\n");
				OC_reallocate_array(c_ptr);
				printf("allocated\n");
			}
			printf("done with allocation\n");
			OC_apply_helper(c_ptr, (OC_apply_template_fp_t)OC_take_value_from_left, NULL, APPLY_INTERNAL, result.index + 1, c_ptr->size, 1);
			printf("apply helper done\n");
			index_for_insert = result.index;
	}
	c_ptr->array[index_for_insert] = (void*)data_ptr;
	c_ptr->size++;
	printf("inserted into array\n");
	g_Container_items_in_use++;
}

/* Return a pointer to an item that points to data equal to the data object pointed to by data_ptr,
using the ordering function to do the comparison with data_ptr as the first argument.
The data_ptr object is assumed to be of the same type as the data objects pointed to by container items.
NULL is returned if no matching item is found. If more than one matching item is present, it is
unspecified which one is returned. The pointed-to data will not be modified. */
void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
	return OC_find_item_arg(c_ptr, data_ptr, c_ptr->comp_fun);
}

/* Return a pointer to the item that points to data that matches the supplied argument given by arg_ptr
according to the supplied function, which compares arg_ptr as the first argument with the data pointer
in each item. This function does not require that arg_ptr be of the same type as the data objects, and
so allows the container to be searched without creating a complete data object first.
NULL is returned if no matching item is found. If more than one matching item is present, it is
unspecified which one is returned. The comparison function must implement an ordering consistent
with the ordering produced by the comparison function specified when the container was created;
if not, the result is undefined. */
void* OC_find_item_arg(const struct Ordered_container* c_ptr, const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
	struct Search_Result result = OC_binary_search(c_ptr, arg_ptr, c_ptr->comp_fun);
	if (result.found)
	{
		return c_ptr->array + result.index;
	}
	else
	{
		return NULL;
	}
}

/* Functions that traverse the items in the container, processing each item in order. */

/* Apply the supplied function to the data pointer in each item of the container.
The contents of the container cannot be modified. */
void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
	OC_apply_helper_simple(c_ptr, (OC_apply_template_fp_t)afp, NULL, APPLY);
}

/* Apply the supplied function to the data pointer in each item in the container.
If the function returns non-zero, the iteration is terminated, and that value
returned. Otherwise, zero is returned. The contents of the container cannot be modified. */
int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
	return OC_apply_helper_simple(c_ptr, (OC_apply_template_fp_t)afp, NULL, APPLY_IF);
}

/* Apply the supplied function to the data pointer in each item in the container;
the function takes a second argument, which is the supplied void pointer.
The contents of the container cannot be modified. */
void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp, void* arg_ptr)
{
	OC_apply_helper_simple(c_ptr, (OC_apply_template_fp_t)afp, arg_ptr, APPLY);
}

/* Apply the supplied function to the data pointer in each item in the container;
the function takes a second argument, which is the supplied void pointer.
If the function returns non-zero, the iteration is terminated, and that value
returned. Otherwise, zero is returned. The contents of the container cannot be modified */
int OC_apply_if_arg(const struct Ordered_container* c_ptr, OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
	return OC_apply_helper_simple(c_ptr, (OC_apply_template_fp_t)afp, arg_ptr, APPLY);
}

/*
Private helper functions
*/

/* Searches for the specified element using the given comparison function */
static struct Search_Result OC_binary_search(const struct Ordered_container* c_ptr, const void* data_ptr, OC_comp_fp_t comp_fun)
{
	struct Search_Result result;
	int left = 0;
	int right = c_ptr->size - 1;
	int middle = (left + right) / 2;
	printf("binary searching\n");
	printf("data ptr = %p\n", data_ptr);
	result.found = 0;
	while (left <= right)
	{
		int comparison = comp_fun(c_ptr->array[middle], data_ptr);
		printf("left = %d\nright = %d\nmiddle = %d\ncomparison = %d\n", left, right, middle, comparison);
		if (comparison < 0)
		{
			left = middle + 1;
		}
		else if (comparison == 0)
		{
			result.found = 1;
			break;
		}
		else
		{
			right = middle - 1;
		}
		middle = (left + right) / 2;
	}
	result.index = middle;
	return result;
}

/* Initialize the container to default values */
static void OC_initialize_container(struct Ordered_container* c_ptr)
{
	c_ptr->allocation = INITIAL_ALLOCATION;
	g_Container_items_allocated += c_ptr->allocation;
	c_ptr->size = 0;
	c_ptr->array = calloc(c_ptr->allocation, sizeof(void**));
}

/* Reallocate array */
static void OC_reallocate_array(struct Ordered_container* c_ptr)
{
	void** old_array = c_ptr->array;
	int i;
	int new_allocation = (c_ptr->allocation + ALLOCATION_INCREASE) * SIZE_FACTOR;
	g_Container_items_allocated += new_allocation - c_ptr->allocation;
	c_ptr->allocation = new_allocation;
	c_ptr->array = calloc(c_ptr->allocation, sizeof(void**));
	for (i = 0; i < c_ptr->size; i++)
	{
		c_ptr->array[i] = old_array[i];
	}
	free(old_array);
}

/* Grabs the data ptr from the item directly preceding this item*/
static int OC_take_value_from_left(void* item_ptr)
{
	item_ptr = *((void**)item_ptr - 1);
	return 0;
}

/* Grabs the data ptr from the item directly after this item*/
static int OC_take_value_from_right(void* item_ptr)
{
	item_ptr = *((void**)item_ptr + 1);
	return 0;
}

/* Helper function for OC_apply functions
Performed on range [start, end), in order depending on reverse */
static int OC_apply_helper(const struct Ordered_container* c_ptr, OC_apply_template_fp_t afp, void* arg_ptr, apply_enum apply_func, int start, int end, int reverse)
{
	int i;
	printf("beginning apply helper\n");
	for (i = (reverse ? end - 1 : start); (reverse ? i >= start : i < end); (reverse ? i-- : i++))
	{
		int function_return;
		void **item_ptr = c_ptr->array[i];
		printf("i = %d\n", i);
		switch (apply_func)
		{
		case APPLY:
			printf("case apply\n");
			((OC_apply_fp_t)afp)(OC_get_data_ptr(item_ptr));
			break;
		case APPLY_IF:
			printf("case apply if\n");
			function_return = ((OC_apply_if_fp_t)afp)(OC_get_data_ptr(item_ptr));
			if (function_return)
			{
				return function_return;
			}
			break;
		case APPLY_ARG:
			printf("case apply arg\n");
			((OC_apply_arg_fp_t)afp)(OC_get_data_ptr(item_ptr), arg_ptr);
			break;
		case APPLY_ARG_IF:
			printf("case apply arg if\n");
			function_return = ((OC_apply_if_arg_fp_t)afp)(OC_get_data_ptr(item_ptr), arg_ptr);
			if (function_return)
			{
				return function_return;
			}
			break;
		case APPLY_INTERNAL:
			printf("case internal\n");
			((OC_apply_internal_fp_t)afp)(item_ptr);
			break;
		}
	}
	return 0;
}

/* Simplified call to OC_apply_helper */
static int OC_apply_helper_simple(const struct Ordered_container* c_ptr, OC_apply_template_fp_t afp, void* arg_ptr, apply_enum apply_func)
{
	return OC_apply_helper(c_ptr, afp, arg_ptr, apply_func, 0, c_ptr->size, 0);
}

#endif
