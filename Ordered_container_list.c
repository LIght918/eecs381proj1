#ifndef ORDERED_CONTAINER_LIST
#define ORDERED_CONTAINER_LIST

#include "Ordered_container.h"
#include "Utility.h"
#include "p1_globals.h"
#include <stdlib.h>

#define CONTAINER_GLOBAL_ADD_ONE 1
#define CONTAINER_GLOBAL_MINUS_ONE -1

/* struct LL_Node structure declaration. This declaration is local to this file. 
This is a two-way or doubly-linked list. Each node has a pointer to the previous 
node and another pointer to the next node in the list. This means insertions or
removals of a node can be made in constant time, once the location has been
determined. */
struct LL_Node { 
    struct LL_Node* prev;      /* pointer to the previous node */
	struct LL_Node* next;		/* pointer to the next node */
	void* data_ptr; 			/* uncommitted pointer to the data item */
};

/* Declaration for Ordered_container. This declaration is local to this file.  
A pointer is maintained to the last node in the list as well as the first, 
meaning that additions to the end of the list can be made in constant time. 
The number of nodes in the list is kept up-to-date in the size member
variable, so that the size of the list can be accessed in constant time. */
struct Ordered_container {
	OC_comp_fp_t comp_func;
	struct LL_Node* first;
	struct LL_Node* last;
	int size;
};

/* Enum for OC_apply functions */
typedef enum { APPLY, APPLY_IF, APPLY_ARG, APPLY_ARG_IF, APPLY_INTERNAL } apply_enum;

/*
Private helper functions declarations
*/

/* Change globals for items in use and items allocated simultaneously */
static void OC_change_globals(int delta);

/* Initialize the container to default values */
static void OC_initialize_container(struct Ordered_container* c_ptr);

/* Deallocates a single node */
static int OC_deallocate_item(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr);

/* Insert node after given node */
static void OC_insert_after(struct Ordered_container* c_ptr, void* item_ptr, const void* data_ptr);

/* Insert node before given node */
static void OC_insert_before(struct Ordered_container* c_ptr, void* item_ptr, const void* data_ptr);

/* Checks if item_ptr's data is equal to arg_ptr, and if so, deletes */
static int OC_check_and_delete(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr);

/* Checks if item_ptr's data is equal to or lesser than arg_ptr, and if so, inserts it before */
static int OC_check_and_insert(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr);

/* Checks if item_ptr's data is equal to arg_ptr, and if so, returns item_ptr */
static int OC_check_and_find(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr);

/* Initialize data for LL_Node* given */
static void OC_initialize_node(struct LL_Node* node_ptr, struct LL_Node* prev, struct LL_Node* next, const void* data_ptr);

/* Type of function used to pass function pointers around OC_apply functions */
typedef void(*OC_apply_template) (void);

/* Type of function used by OC_apply for APPLY_INTERNAL */
typedef int(*OC_apply_internal_fp_t) (struct Ordered_container *c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr);

/* Helper function for OC_apply functions */
static int OC_apply_helper(const struct Ordered_container* c_ptr, OC_apply_template afp, void* arg_ptr, apply_enum apply_func, OC_comp_fp_t comp_func);

/* Deallocate all nodes */
static void OC_deallocate_all(struct Ordered_container *c_ptr);

/*
Functions for the entire container.
*/

/* Create an empty container using the supplied comparison function, and return the pointer to it. */
struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
	struct Ordered_container *c_ptr = malloc(sizeof(struct Ordered_container));
	c_ptr->comp_func = f_ptr;
	OC_initialize_container(c_ptr);
	g_Container_count++;
	return c_ptr;
}

/* Destroy the container and its items; caller is responsible for
deleting all pointed-to data before calling this function.
After this call, the container pointer value must not be used again. */
void OC_destroy_container(struct Ordered_container* c_ptr)
{
	OC_deallocate_all(c_ptr);
	OC_change_globals(c_ptr->size * CONTAINER_GLOBAL_MINUS_ONE);
	g_Container_count--;
	free(c_ptr);
}

/* Delete all the items in the container and initialize it.
Caller is responsible for deleting any pointed-to data first. */
void OC_clear(struct Ordered_container* c_ptr)
{
	OC_deallocate_all(c_ptr);
	OC_change_globals(c_ptr->size * CONTAINER_GLOBAL_MINUS_ONE);
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
	return ((struct LL_Node*)item_ptr)->data_ptr;
}

/* Delete the specified item.
Caller is responsible for any deletion of the data pointed to by the item. */
void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
	struct LL_Node *node_ptr = (struct LL_Node*)item_ptr;
	if (node_ptr->next == NULL && node_ptr->prev == NULL)
	{
		/* container must now be empty */
		OC_initialize_container(c_ptr);
	}
	else
	{
		if (node_ptr->prev != NULL)
		{
			node_ptr->prev->next = node_ptr->next;
		}
		else
		{
			c_ptr->first = node_ptr->next;
		}

		if (node_ptr->next != NULL)
		{
			node_ptr->next->prev = node_ptr->prev;
		}
		else
		{
			c_ptr->last = node_ptr->prev;
		}
		c_ptr->size--;
	}
	OC_change_globals(CONTAINER_GLOBAL_MINUS_ONE);
	OC_deallocate_item(c_ptr, item_ptr, NULL, NULL);
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
	int is_inserted = OC_apply_helper(c_ptr, (OC_apply_template)OC_check_and_insert, (void*)data_ptr, APPLY_INTERNAL, c_ptr->comp_func);
	if (is_inserted == 0)
	{
		OC_insert_after(c_ptr, c_ptr->last, data_ptr);
	}
	OC_change_globals(CONTAINER_GLOBAL_ADD_ONE);
}

/* Return a pointer to an item that points to data equal to the data object pointed to by data_ptr,
using the ordering function to do the comparison with data_ptr as the first argument.
The data_ptr object is assumed to be of the same type as the data objects pointed to by container items.
NULL is returned if no matching item is found. If more than one matching item is present, it is
unspecified which one is returned. The pointed-to data will not be modified. */
void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
	return OC_apply_helper(c_ptr, (OC_apply_template)OC_check_and_find, (void*)data_ptr, APPLY_INTERNAL, c_ptr->comp_func);
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
	return OC_apply_helper(c_ptr, (OC_apply_template)OC_check_and_find, data_ptr, APPLY_INTERNAL, fafp);
}

/* Functions that traverse the items in the container, processing each item in order. */

/* Apply the supplied function to the data pointer in each item of the container.
The contents of the container cannot be modified. */
void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
	OC_apply_helper(c_ptr, (OC_apply_template)afp, NULL, APPLY, NULL);
}

/* Apply the supplied function to the data pointer in each item in the container.
If the function returns non-zero, the iteration is terminated, and that value
returned. Otherwise, zero is returned. The contents of the container cannot be modified. */
int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
	return OC_apply_helper(c_ptr, (OC_apply_template)afp, NULL, APPLY_IF, NULL);
}

/* Apply the supplied function to the data pointer in each item in the container;
the function takes a second argument, which is the supplied void pointer.
The contents of the container cannot be modified. */
void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp, void* arg_ptr)
{
	OC_apply_helper(c_ptr, (OC_apply_template)afp, arg_ptr, APPLY_ARG, NULL);
}

/* Apply the supplied function to the data pointer in each item in the container;
the function takes a second argument, which is the supplied void pointer.
If the function returns non-zero, the iteration is terminated, and that value
returned. Otherwise, zero is returned. The contents of the container cannot be modified */
int OC_apply_if_arg(const struct Ordered_container* c_ptr, OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
	return OC_apply_helper(c_ptr, (OC_apply_template)afp, arg_ptr, APPLY_ARG_IF, NULL);
}

/*
Private helper functions
*/

/* Change globals for items in use and items allocated simultaneously */
static void OC_change_globals(int delta)
{
	g_Container_items_allocated - += delta;
	g_Container_items_in_use += delta;
}

/* Initialize the container to default values */
static void OC_initialize_container(struct Ordered_container* c_ptr)
{
	c_ptr->first = NULL;
	c_ptr->last = NULL;
	c_ptr->size = 0;
}

/* Deallocates a single node */
static int OC_deallocate_item(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr)
{
	free(item_ptr);
	return 0;
}

/* Insert node after given node */
static void OC_insert_after(struct Ordered_container* c_ptr, void* item_ptr, const void* data_ptr)
{
	struct LL_Node *node_ptr = (struct LL_Node*)item_ptr;
	struct LL_Node *new_node = malloc(sizeof(struct LL_Node));
	if (node_ptr->next == NULL)
	{
		OC_initialize_node(new_node, node_ptr, NULL, data_ptr);
		node_ptr->next = new_node;
		c_ptr->last = new_node;
	}
	else
	{
		OC_initialize_node(new_node, node_ptr, node_ptr->next, data_ptr);
		node_ptr->next->prev = new_node;
		node_ptr->next = new_node;
	}
}

/* Insert node before given node */
static void OC_insert_before(struct Ordered_container* c_ptr, void* item_ptr, const void* data_ptr)
{
	struct LL_Node *node_ptr = (struct LL_Node*)item_ptr;
	struct LL_Node *new_node = malloc(sizeof(struct LL_Node));
	if (node_ptr->prev == NULL)
	{
		OC_initialize_node(new_node, NULL, node_ptr, data_ptr);
		node_ptr->prev = new_node;
		c_ptr->first = new_node;
	}
	else
	{
		OC_initialize_node(new_node, node_ptr->prev, node_ptr, data_ptr);
		node_ptr->prev->next = new_node;
		node_ptr->prev = new_node;
	}
}

/* Checks if item_ptr's data is equal to arg_ptr, and if so, deletes */
static int OC_check_and_delete(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr)
{
	if (comp_func(OC_get_data_ptr(item_ptr), arg_ptr) == 0)
	{
		OC_delete_item(c_ptr, item_ptr);
		return 1;
	}
	return 0;
}

/* Checks if item_ptr's data is equal to or lesser than arg_ptr, and if so, inserts it before */
static int OC_check_and_insert(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr)
{
	if (comp_func(OC_get_data_ptr(item_ptr), arg_ptr) >= 0)
	{
		OC_insert_before(c_ptr, item_ptr, arg_ptr);
		return 1;
	}
	return 0;
}

/* Checks if item_ptr's data is equal to arg_ptr, and if so, returns item_ptr */
static int OC_check_and_find(struct Ordered_container* c_ptr, void* item_ptr, OC_comp_fp_t comp_func, const void* arg_ptr)
{
	if (comp_func(OC_get_data_ptr(item_ptr), arg_ptr) == 0)
	{
		return (int)item_ptr;
	}
	return 0;
}

/* Initialize data for LL_Node* given */
static void OC_initialize_node(struct LL_Node* node_ptr, struct LL_Node* prev, struct LL_Node* next, const void* data_ptr)
{
	node_ptr->prev = prev;
	node_ptr->next = next;
	node_ptr->data_ptr = data_ptr;
}

/* Helper function for OC_apply functions */
static int OC_apply_helper(const struct Ordered_container* c_ptr, OC_apply_template afp, void* arg_ptr, apply_enum apply_func, OC_comp_fp_t comp_func)
{
	struct LL_Node *node_ptr = c_ptr->first;
	while (node_ptr != NULL)
	{
		struct LL_Node *next_node_ptr = node_ptr->next;
		int function_return;
		switch (apply_func)
		{
		case APPLY:
			((OC_apply_fp_t)afp)(OC_get_data_ptr(node_ptr));
			break;
		case APPLY_IF:
			function_return = ((OC_apply_if_fp_t)afp)(OC_get_data_ptr(node_ptr));
			if (function_return)
			{
				return function_return;
			}
			break;
		case APPLY_ARG:
			((OC_apply_arg_fp_t)afp)(OC_get_data_ptr(node_ptr), arg_ptr);
			break;
		case APPLY_ARG_IF:
			function_return = ((OC_apply_arg_if_fp_t)afp)(OC_get_data_ptr(node_ptr), arg_ptr);
			if (function_return)
			{
				return function_return;
			}
			break;
		case APPLY_INTERNAL:
			function_return = ((OC_apply_internal_fp_t)afp)(c_ptr, node_ptr, comp_func, arg_ptr);
			if (function_return)
			{
				return function_return;
			}
			break;
		}
		node_ptr = next_node_ptr;
	}
	return 0;
}

/* Deallocate all nodes */
static void OC_deallocate_all(struct Ordered_container *c_ptr)
{
	OC_apply_helper(c_ptr, OC_deallocate_item, NULL, APPLY_INTERNAL, NULL);
}

#endif
