#include "Collection.h"
#include "Record.h"
#include "Ordered_container.h"
#include "Utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "p1_globals.h"

/* a Collection contains a pointer to a C-string name and a container
that holds pointers to Records - the members. */
struct Collection {
	char* name;
	struct Ordered_container* members; 
};

/* Used to save all members of the collection */
void save_one_record(void* record, void* current_file);

/* Create a Collection object.
This is the only function that allocates memory for a Collection
and the contained data. */
struct Collection* create_Collection(const char* name)
{
	struct Collection *collection = malloc(sizeof(struct Collection));
	int name_len = strlen(name) + 1;
	g_string_memory += name_len;
	collection->name = strcpy(malloc(name_len), name);
	collection->members = OC_create_container(record_compare_title);
	return collection;
}

/* Destroy a Collection object.
This is the only function that frees the memory for a Collection
and the contained data. It discards the member list,
but of course does not delete the Records themselves. */
void destroy_Collection(struct Collection* collection_ptr)
{
	g_string_memory -= strlen(collection_ptr->name) + 1;
	free(collection_ptr->name);
	OC_destroy_container(collection_ptr->members);
	free(collection_ptr);
}

/* Return tthe collection name. */
const char* get_Collection_name(const struct Collection* collection_ptr)
{
	return collection_ptr->name;
}

/* return non-zero if there are no members, 0 if there are members */
int Collection_empty(const struct Collection* collection_ptr)
{
	return OC_empty(collection_ptr->members);
}

/* Add a member; return non-zero and do nothing if already present. */
int add_Collection_member(struct Collection* collection_ptr, const struct Record* record_ptr)
{
	if (!is_Collection_member_present(collection_ptr, record_ptr))
	{
		OC_insert(collection_ptr->members, record_ptr);
		return 0;
	}
	return 1;
}

/* Return non-zero if the record is a member, zero if not. */
int is_Collection_member_present(const struct Collection* collection_ptr, const struct Record* record_ptr)
{
	return OC_find_item(collection_ptr->members, record_ptr) != 0;
}

/* Remove a member; return non-zero if not present, zero if was present. */
int remove_Collection_member(struct Collection* collection_ptr, const struct Record* record_ptr)
{
	if (is_Collection_member_present(collection_ptr, record_ptr))
	{
		OC_delete_item(collection_ptr->members, (void*)record_ptr);
		return 0;
	}
	return 1;
}

/* Print the data in a Collection. */
void print_Collection(const struct Collection* collection_ptr)
{
	printf("Collection %s contains:", collection_ptr->name);
	if (!Collection_empty(collection_ptr))
	{
		printf("\n");
		OC_apply(collection_ptr->members, record_print);
	}
	else
	{
		printf(" None\n");
	}
}

/* Write the data in a Collection to a file. */
void save_Collection(const struct Collection* collection_ptr, FILE* outfile)
{
	fprintf(outfile, "%s %d\n", collection_ptr->name, OC_get_size(collection_ptr->members));
	OC_apply_arg(collection_ptr->members, save_one_record, outfile);
}

/* Read a Collection's data from a file stream, create the data object and
return a pointer to it, NULL if invalid data discovered in file.
No check made for whether the Collection already exists or not. */
struct Collection* load_Collection(FILE* input_file, const struct Ordered_container* records)
{
	struct Collection *collection;
	char collection_name[BUFFER_SIZE];
	int elements = 0;
	if (fscanf(input_file, SCAN_BUFFER, collection_name) != 1)
	{
		/* error reading name */
		printf("error reading name\n");
		return NULL;
	}
	if (fscanf(input_file, "%d\n", &elements) != 1)
	{
		/* error reading size */
		printf("error reading size\n");
		return NULL;
	}
	collection = create_Collection(collection_name);
	for (; elements > 0; elements--)
	{
		char title_buffer[BUFFER_SIZE];
		char *title = read_title(title_buffer, input_file);
		void *item;
		if (title == NULL)
		{
			/* error reading record title */
			printf("error reading record title\n");
			destroy_Collection(collection);
			return NULL;
		}
		item = OC_find_item_arg(records, title, record_title_compare);
		if (item == NULL)
		{
			/* title not found in library */
			printf("error title not found in library\n");
			destroy_Collection(collection);
			return NULL;
		}
		OC_insert(collection->members, OC_get_data_ptr(item));
	}
	return collection;
}

/* Used to save all members of the collection */
void save_one_record(void* record, void* current_file)
{
	fprintf((FILE*)current_file, "%s\n", get_Record_title((struct Record *)record));
}