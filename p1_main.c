#include <stdio.h>
#include <string.h>
#include "p1_globals.h"
#include "Collection.h"
#include "Record.h"
#include "Ordered_container.h"
#include "Utility.h"

/* Compare collections by their name */
int collection_compare(const struct Collection *first_collection, const struct Collection *second_collection);

/* Compares a collection's title with the given title */
int collection_name_compare(const char * name, const struct Collection *collection);

/* Used to print all members of a container containing collections */
void print_all_collections(struct Collection *collection);

/* Read in title and get record from library */
struct Record * read_title_get_record(struct Ordered_container *library_title);

/* Read record id and get record from library */
struct Record * read_id_get_record(struct Ordered_container *library_id);

/* Read in name and get collection from library */
struct Collection * read_name_get_collection(struct Ordered_container *catalog);

/* Return non-zero if there are no members, 0 if there are members */
int Collection_not_empty(const struct Collection* collection_ptr);

/* Used to deallocate all members of an Ordered container */
void free_member(void * addr);

/* Deallocates all members and clears the container*/
void deallocate_and_clear(struct Ordered_container *container);

/* Clear all data */
void clear_all(struct Ordered_container *catalog, struct Ordered_container *library_title, struct Ordered_container *library_id);

/* Action Object input error message */
void action_object_input_error();

/* Title read error message */
void title_read_error();

/* Integer read error message */
void integer_read_error();

/* File open error message */
void file_open_error();

/* File invalid error message and closes file */
void file_invalid_error(FILE *file);

int main()
{
	struct Ordered_container *catalog = OC_create_container(collection_compare);
	struct Ordered_container *library_title = OC_create_container(record_compare_title);
	struct Ordered_container *library_id = OC_create_container(record_compare_id);
	char action, object;
	while (true)
	{
		printf("\nEnter command: ");
		if (scanf("%c%c", action, object) != 2)
		{
			switch (action)
			{
			case 'f': /* find (records only) */
				switch (object)
				{
				case 'r': /* find record */
					struct Record *item = read_title_get_record(library_title);
					if (item)
					{
						print_Record(item);
					}
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'p': /* print */
				switch (object)
				{
				case 'r': /* print record */
					int id;
					struct Record *item = read_id_get_record(library_id);
					if (!item)
					{
						break;
					}
					print_Record(item);
					break;
				case 'c': /* print collection */
					struct Collection *collection = read_name_get_collection(catalog);
					if (!collection)
					{
						break;
					}
					print_Collection(collection);
					break;
				case 'L': /* print library */
					if (!OC_empty(library_title))
					{
						printf("Library contains %d records:\n");
						OC_apply(library_title, print_Record);
					}
					else
					{
						printf("Library is empty\n");
					}
					break;
				case 'C': /* print catalog */
					if (!OC_empty(catalog))
					{
						printf("Catalog contains %d collections:\n");
						OC_apply(catalog, print_Collection);
					}
					else
					{
						printf("Catalog is empty\n");
					}
					break;
				case 'a': /* print memory allocations */
					printf("Memory allocations:\n");
					printf("Records: %d\n", g_Record_count);
					printf("Collections: %d\n", g_Collection_count);
					printf("Containers: %d\n", g_Container_count);
					printf("Container items in use: %d\n", g_Container_items_in_use);
					printf("Container items allocated: %d\n", g_Container_items_allocated);
					printf("C-strings: %d bytes total\n", g_string_memory);
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'm': /* modify (rating only) */
				switch (object)
				{
				case 'r': /* modify rating of a record */
					int id, rating;
					struct Record *item = read_id_get_record(library_id);
					if (scanf("%d", rating) != 1)
					{
						integer_read_error();
						break;
					}
					if (rating < 0 || rating > 10)
					{
						printf("Rating is out of range!\n");
						break;
					}
					set_Record_rating(item, rating);
					printf("Rating for record %d changed to %d\n", id, rating);
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'a': /* add */
				switch (object)
				{
				case 'r': /* add record */
					char medium[BUFFER_SIZE];
					char title_buffer[BUFFER_SIZE];
					char *title;
					struct Record *record;
					if (scanf("%SCAN_BUFFER_SIZEs", medium) != 1)
					{
						title_read_error();
						break;
					}
					title = read_title(title_buffer, stdin);
					if (!title)
					{
						title_read_error();
						break;
					}
					if (OC_find_item_arg(library_title, title, record_title_compare) != 0)
					{
						printf("Library already has a record with this title!\n");
						break;
					}
					record = create_Record(medium, title);
					OC_insert(library_title, record);
					OC_insert(library_id, record);
					printf("Record %d added\n", get_Record_ID(record));
					break;
				case 'c': /* add collection */
					char name[BUFFER_SIZE];
					struct Collection *collection;
					if (scanf("%SCAN_BUFFER_SIZEs", name) != 1)
					{
						title_read_error();
						break;
					}
					if (OC_find_item_arg(catalog, name, collection_name_compare) != 0)
					{
						printf("Catalog already has a collection with this name!\n");
						break;
					}
					collection = create_Collection(name);
					OC_insert(catalog, collection);
					printf("Collection %s added\n", name);
					break;
				case 'm': /* add record to collection */
					struct Collection *collection = read_name_get_collection(catalog);
					struct Record *item = read_title_get_record(library_title);
					if (!item || !collection)
					{
						break;
					}
					if (!add_Collection_member(collection, item))
					{
						printf("Member %d %s added\n", get_Record_ID(item), get_Record_title(item));
					}
					else
					{
						printf("Record is already a member in the collection!\n");
					}
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'd': /* delete */
				switch (object)
				{
				case 'r': /* delete record */
					struct Record *item = read_title_get_record(library_title);
					int is_member;
					if (!item)
					{
						break;
					}
					is_member = OC_apply_if_arg(catalog, is_Collection_member_present, item);
					if (is_member)
					{
						printf("Cannot delete a record that is a member of a collection!\n");
						break;
					}
					OC_delete_item(library_title, item);
					OC_delete_item(library_id, item);
					printf("Record %d %s deleted\n", get_Record_id(item), get_Record_title(item));
					break;
				case 'c': /* delete collection */
					struct Collection *collection = read_name_get_collection(catalog);
					if (!collection)
					{
						break;
					}
					OC_delete_item(catalog, collection);
					printf("Collection %s deleted\n", get_Collection_name(collection));
					break;
				case 'm': /* delete record from collection */
					struct Collection *collection = read_name_get_collection(catalog);
					struct Record *item = read_title_get_record(library_title);
					if (!item || !collection)
					{
						break;
					}
					if (!remove_Collection_member(collection, item))
					{
						printf("Member %d %s deleted\n", get_Record_id(item), get_Record_title(item));
					}
					else
					{
						printf("Record is not a member in the collection!\n");
					}
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'c': /* clear */
				switch (object)
				{
				case 'L': /* clear library */
					int has_members = OC_apply_if(catalog, Collection_not_empty);
					if (has_members)
					{
						printf("Cannot clear all records unless all collections are empty!\n");
						break;
					}
					deallocate_and_clear(library_title);
					deallocate_and_clear(library_id);
					reset_Record_ID_counter();
					printf("All records deleted\n");
					break;
				case 'C': /* clear catalog */
					deallocate_and_clear(catalog);
					printf("All collections deleted\n");
					break;
				case 'A': /* clear all */
					clear_all(catalog, library_title, library_id);
					printf("All data deleted\n");
				default:
					action_object_input_error();
					break;
				}
			case 's': /* save */
				switch (object)
				{
				case 'A': /* save all */
					char filename[BUFFER_SIZE];
					FILE *outfile;
					if (scanf("%SCAN_BUFFER_SIZEs", filename) != 1)
					{
						file_open_error();
						return NULL;
					}
					outfile = fopen(outfile, "w");
					if (!outfile)
					{
						file_open_error();
						break;
					}
					fprintf(outfile, "%d\n", OC_get_size(library_title));
					OC_apply_arg(library_title, save_Record, outfile);
					OC_apply_arg(catalog, save_Collection, outfile);
					fclose(outfile);
					printf("Data saved\n");
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'r': /* restore */
				switch (object)
				{
				case 'A': /* restore all */
					int records, collections;
					char filename[BUFFER_SIZE];
					FILE *infile;
					if (scanf("%SCAN_BUFFER_SIZEs", filename) != 1)
					{
						file_open_error();
						return NULL;
					}
					infile = fopen(outfile, "w");
					if (!infile)
					{
						file_open_error();
						break;
					}
					clear_all(catalog, library_title, library_id);
					if (fscanf(infile, "%d\n", records) != 1)
					{
						file_invalid_error(infile);
						break;
					}
					for (; records > 0; records--)
					{
						struct Record *record = load_Record(infile);
						if (!record)
						{
							/* error loading a record */
							break;
						}
						OC_insert(library_title, record);
						OC_insert(library_id, record);
					}
					if (records > 0 || fscanf(infile, "%d\n", collections) != 1)
					{
						file_invalid_error(infile);
						clear_all(catalog, library_title, library_id);
						break;
					}
					for (; collections > 0; collections--)
					{
						struct Collection *collection = load_Collection(infile, library_title);
						if (!collection)
						{
							/* error loading a collection */
							break;
						}
						OC_insert(catalog, collection);
					}
					if (collections > 0)
					{
						file_invalid_error(infile);
						clear_all(catalog, library_title, library_id);
						break;
					}
					printf("Data loaded\n");
					break;
				default:
					action_object_input_error();
					break;
				}
			case 'q': /* quit */
				switch (object)
				{
				case 'q': /* quit */
					printf("Done\n");
					return 0;
				default:
					action_object_input_error();
					break;
				}
			default:
				action_object_input_error();
				break;
			}
		}
	}
}

/* Compare collections by their name */
int collection_compare(const struct Collection *first_collection, const struct Collection *second_collection)
{
	return strcomp(get_Collection_name(first_collection), get_Collection_name(second_collection));
}

/* Compares a collection's title with the given title */
int collection_name_compare(const char * name, const struct Collection *collection)
{
	return strcomp(name, get_Collection_name(collection));
}

/* Read in title and get record from library */
struct Record * read_title_get_record(struct Ordered_container *library_title)
{
	char title_buffer[BUFFER_SIZE];
	char *title = read_title(title_buffer, stdin);
	struct Record *item;
	if (!title)
	{
		title_read_error();
		return NULL;
	}
	item = OC_find_item_arg(library_title, title, record_title_compare);
	if (!item)
	{
		printf("No record with that title!\n");
	}
	return item;
}

/* Read record id and get record from library */
struct Record * read_id_get_record(struct Ordered_container *library_id)
{
	int id;
	struct Record *item;
	if (scanf("%d", id) != 1)
	{
		integer_read_error();
		break;
	}
	item = OC_find_item_arg(library_id, &id, record_id_compare);
	if (!item)
	{
		printf("No record with that ID!\n");
	}
	return item;
}

/* Read in name and get collection from library */
struct Collection * read_name_get_collection(struct Ordered_container *catalog)
{
	char name[BUFFER_SIZE];
	struct Collection *collection;
	if (scanf("%SCAN_BUFFER_SIZEs", name) != 1)
	{
		/* couldn't read name */
		return NULL;
	}
	collection = OC_find_item_arg(catalog, name, collection_name_compare);
	if (!collection)
	{
		/* collection not found error */
	}
	return collection;
}

/* Return non-zero if there are no members, 0 if there are members */
int Collection_not_empty(const struct Collection* collection_ptr)
{
	return !Collection_empty(collection_ptr);
}

/* Used to deallocate all members of an Ordered container */
void free_member(void * addr)
{
	free(addr);
}

/* Deallocates all members and clears the container*/
void deallocate_and_clear(Ordered_container *container)
{
	OC_apply(container, free_member);
	OC_clear(container);
}

/* Clear all data */
void clear_all(struct Ordered_container *catalog, struct Ordered_container *library_title, struct Ordered_container *library_id)
{
	deallocate_and_clear(catalog);
	deallocate_and_clear(library_title);
	deallocate_and_clear(library_id);
	reset_Record_ID_counter();
}

/* Action Object input error message */
void action_object_input_error()
{
	printf("Unrecognized command!\n");
}

/* Title read error message */
void title_read_error()
{
	printf("Could not read a title!\n");
}

/* Integer read error message */
void integer_read_error()
{
	printf("Could not read an integer value!\n");
}

/* File open error message */
void file_open_error()
{
	printf("Could not open file!\n");
}

/* File invalid error message and closes file */
void file_invalid_error(FILE *file)
{
	printf("Invalid data found in file!\n");
	fclose(file);
}