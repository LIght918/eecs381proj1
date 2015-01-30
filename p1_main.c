#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p1_globals.h"
#include "Collection.h"
#include "Record.h"
#include "Ordered_container.h"
#include "Utility.h"

/* Safely acquires data ptr of an item ptr */
void *OC_safe_data_ptr(void *item_ptr);

/* Compare collections by their name */
int collection_compare(const void* first_collection, const void* second_collection);

/* Compares a collection's title with the given title */
int collection_name_compare(const void* name, const void* collection);

/* Print a collection */
void collection_print(void* collection);

/* Used to print all members of a container containing collections */
void print_all_collections(struct Ordered_container *c_ptr);

/* Checks collection if it contains member */
int collection_contains(void* collection_ptr, void* record_ptr);

/* Save a collection */
void collection_save(void* collection, void* current_file);

/* Save a record */
void record_save(void* record, void* current_file);

/* Read in title and get item ptr to record from library */
void *read_title_get_item_ptr(struct Ordered_container *library_title);

/* Read in title and get record from library */
struct Record * read_title_get_record(struct Ordered_container *library_title);

/* Read record id and get record from library */
struct Record * read_id_get_record(struct Ordered_container *library_id);

/* Read in name and get item ptr to collection from library */
void *read_name_get_item_ptr(struct Ordered_container *catalog);

/* Read in name and get collection from library */
struct Collection * read_name_get_collection(struct Ordered_container *catalog);

/* Return non-zero if there are no members, 0 if there are members */
int Collection_not_empty(void* collection_ptr);

/* Used to destroy all records in an Ordered container */
void record_destroy(void * addr);

/* Used to destroy all collections in an Ordered container */
void collection_destroy(void * addr);

/* Clear catalog */
void clear_catalog(struct Ordered_container *catalog);

/* Clear library */
void clear_library(struct Ordered_container *library_title, struct Ordered_container *library_id);

/* Clear all data */
void clear_all(struct Ordered_container *catalog, struct Ordered_container *library_title, struct Ordered_container *library_id);

/* Clear all and print a message */
void clear_all_message(struct Ordered_container *catalog, struct Ordered_container *library_title, struct Ordered_container *library_id);

/* Reads in filename and open file with given mode */
FILE * read_filename_open_file(char * mode);

/* Flushes the stream until the next \n */
void flush_stream(void);

/* Prints message and flushes */
void message_and_error(char * message);

/* Prints message and flushes */
void message_and_error_noflush(char * message);

/* Action Object input error message */
void action_object_input_error(void);

/* Title read error message */
void title_read_error(void);

/* Integer read error message */
void integer_read_error(void);

/* File open error message */
void file_open_error(void);

/* File invalid error message and closes file */
void file_invalid_error(FILE *file);

int main()
{
	struct Ordered_container *catalog = OC_create_container(collection_compare);
	struct Ordered_container *library_title = OC_create_container(record_compare_title);
	struct Ordered_container *library_id = OC_create_container(record_compare_id);
	char action, object;
	while (1)
	{
		printf("\nEnter command: ");
		if (scanf(" %c %c", &action, &object) == 2)
		{
			switch (action)
			{
				case 'f': /* find (records only) */
				{
					switch (object)
					{
						case 'r': /* find record */
						{
							struct Record *item = read_title_get_record(library_title);
							if (item)
							{
								print_Record(item);
							}
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'p': /* print */
				{
					switch (object)
					{
						case 'r': /* print record */
						{
							struct Record *item = read_id_get_record(library_id);
							if (!item)
							{
								break;
							}
							print_Record(item);
							break;
						}
						case 'c': /* print collection */
						{
							struct Collection *collection = read_name_get_collection(catalog);
							if (!collection)
							{
								break;
							}
							print_Collection(collection);
							break;
						}
						case 'L': /* print library */
						{
							if (!OC_empty(library_title))
							{
								printf("Library contains %d records:\n", OC_get_size(library_title));
								OC_apply(library_title, record_print);
							}
							else
							{
								printf("Library is empty\n");
							}
							break;
						}
						case 'C': /* print catalog */
						{
							if (!OC_empty(catalog))
							{
								printf("Catalog contains %d collections:\n", OC_get_size(catalog));
								print_all_collections(catalog);
							}
							else
							{
								printf("Catalog is empty\n");
							}
							break;
						}
						case 'a': /* print memory allocations */
						{
							printf("Memory allocations:\n");
							printf("Records: %d\n", OC_get_size(library_title));
							printf("Collections: %d\n", OC_get_size(catalog));
							printf("Containers: %d\n", g_Container_count);
							printf("Container items in use: %d\n", g_Container_items_in_use);
							printf("Container items allocated: %d\n", g_Container_items_allocated);
							printf("C-strings: %d bytes total\n", g_string_memory);
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'm': /* modify (rating only) */
				{
					switch (object)
					{
						case 'r': /* modify rating of a record */
						{
							int rating;
							struct Record *item = read_id_get_record(library_id);
							if (scanf("%d", &rating) != 1)
							{
								integer_read_error();
								break;
							}
							if (rating < RATING_MIN || rating > RATING_MAX)
							{
								message_and_error("Rating is out of range!\n");
								break;
							}
							set_Record_rating(item, rating);
							printf("Rating for record %d changed to %d\n", get_Record_ID(item), rating);
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'a': /* add */
				{
					switch (object)
					{
						case 'r': /* add record */
						{
							char medium[MEDIUM_BUFFER_SIZE];
							char title_buffer[TITLE_BUFFER_SIZE];
							char *title;
							struct Record *record;
							if (scanf(MEDIUM_SCAN_BUFFER, medium) != 1)
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
								message_and_error_noflush("Library already has a record with this title!\n");
								break;
							}
							record = create_Record(medium, title);
							OC_insert(library_title, record);
							OC_insert(library_id, record);
							printf("Record %d added\n", get_Record_ID(record));
							break;
						}
						case 'c': /* add collection */
						{
							char name[NAME_BUFFER_SIZE];
							struct Collection *collection;
							if (scanf(NAME_SCAN_BUFFER, name) != 1)
							{
								title_read_error();
								break;
							}
							if (OC_find_item_arg(catalog, name, collection_name_compare) != 0)
							{
								message_and_error("Catalog already has a collection with this name!\n");
								break;
							}
							collection = create_Collection(name);
							OC_insert(catalog, collection);
							printf("Collection %s added\n", name);
							break;
						}
						case 'm': /* add record to collection */
						{
							struct Collection *collection = read_name_get_collection(catalog);
							struct Record *record;
							if (!collection)
							{
								break;
							}
							record = read_id_get_record(library_id);
							if (!record)
							{
								break;
							}
							if (!add_Collection_member(collection, record))
							{
								printf("Member %d %s added\n", get_Record_ID(record), get_Record_title(record));
							}
							else
							{
								message_and_error("Record is already a member in the collection!\n");
							}
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'd': /* delete */
				{
					switch (object)
					{
						case 'r': /* delete record */
						{
							void *item = read_title_get_item_ptr(library_title);
							struct Record *record = OC_safe_data_ptr(item);
							if (!item)
							{
								break;
							}
							if (OC_apply_if_arg(catalog, collection_contains, record))
							{
								message_and_error("Cannot delete a record that is a member of a collection!\n");
								break;
							}
							OC_delete_item(library_title, item);
							OC_delete_item(library_id, OC_find_item(library_id, record));
							printf("Record %d %s deleted\n", get_Record_ID(record), get_Record_title(record));
							destroy_Record(record);
							break;
						}
						case 'c': /* delete collection */
						{
							void *item = read_name_get_item_ptr(catalog);
							struct Collection *collection;
							if (!item)
							{
								break;
							}
							collection = OC_get_data_ptr(item);
							OC_delete_item(catalog, item);
							printf("Collection %s deleted\n", get_Collection_name(collection));
							destroy_Collection(collection);
							break;
						}
						case 'm': /* delete record from collection */
						{
							struct Collection *collection = read_name_get_collection(catalog);
							struct Record *record;
							if (!collection)
							{
								break;
							}
							record = read_id_get_record(library_id);
							if (!record)
							{
								break;
							}
							if (!remove_Collection_member(collection, record))
							{
								printf("Member %d %s deleted\n", get_Record_ID(record), get_Record_title(record));
							}
							else
							{
								message_and_error("Record is not a member in the collection!\n");
							}
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'c': /* clear */
				{
					switch (object)
					{
						case 'L': /* clear library */
						{
							if (OC_apply_if(catalog, Collection_not_empty);)
							{
								message_and_error("Cannot clear all records unless all collections are empty!\n");
								break;
							}
							clear_library(library_title, library_id);
							printf("All records deleted\n");
							break;
						}
						case 'C': /* clear catalog */
						{
							clear_catalog(catalog);
							printf("All collections deleted\n");
							break;
						}
						case 'A': /* clear all */
						{
							clear_all_message(catalog, library_title, library_id);
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 's': /* save */
				{
					switch (object)
					{
						case 'A': /* save all */
						{
							FILE *outfile = read_filename_open_file("w");
							if (!outfile)
							{
								break;
							}
							fprintf(outfile, "%d\n", OC_get_size(library_title));
							OC_apply_arg(library_title, record_save, outfile);
							fprintf(outfile, "%d\n", OC_get_size(catalog));
							OC_apply_arg(catalog, collection_save, outfile);
							fclose(outfile);
							printf("Data saved\n");
							break;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'r': /* restore */
				{
					switch (object)
					{
						case 'A': /* restore all */
						{
							int records, collections;
							FILE *infile = read_filename_open_file("r");
							if (!infile)
							{
								break;
							}
							clear_all(catalog, library_title, library_id);
							if (fscanf(infile, "%d\n", &records) != 1)
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
							if (records > 0 || fscanf(infile, "%d\n", &collections) != 1)
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
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				case 'q': /* quit */
				{
					switch (object)
					{
						case 'q': /* quit */
						{
							clear_all_message(catalog, library_title, library_id);
							printf("Done\n");
							return 0;
						}
						default:
						{
							action_object_input_error();
							break;
						}
					}
					break;
				}
				default:
				{
					action_object_input_error();
					break;
				}
			}
		}
	}
	OC_destroy_container(catalog);
	OC_destroy_container(library_title);
	OC_destroy_container(library_id);
}

/* Safely acquires data ptr of an item ptr */
void *OC_safe_data_ptr(void *item_ptr)
{
	if (item_ptr == NULL)
	{
		return NULL;
	}
	return OC_get_data_ptr(item_ptr);
}

/* Compare collections by their name */
int collection_compare(const void* first_collection, const void* second_collection)
{
	return strcmp(get_Collection_name(first_collection), get_Collection_name(second_collection));
}

/* Compares a collection's title with the given title */
int collection_name_compare(const void* name, const void* collection)
{
	return strcmp((const char*)name, get_Collection_name((const struct Collection*)collection));
}

/* Print a collection */
void collection_print(void* collection)
{
	print_Collection((struct Collection *)collection);
}

/* Used to print all members of a container containing collections */
void print_all_collections(struct Ordered_container *c_ptr)
{
	OC_apply(c_ptr, collection_print);
}

/* Checks collection if it contains member */
int collection_contains(void* collection_ptr, void* record_ptr)
{
	return is_Collection_member_present((struct Collection *) collection_ptr, (struct Record *)record_ptr);
}

/* Save a collection */
void collection_save(void* collection, void* current_file)
{
	save_Collection((struct Collection *)collection, (FILE *)current_file);
}

/* Save a record */
void record_save(void* record, void* current_file)
{
	save_Record((struct Record *)record, (FILE *)current_file);
}

/* Read in title and get item ptr to record from library */
void *read_title_get_item_ptr(struct Ordered_container *library_title)
{
	char title_buffer[TITLE_BUFFER_SIZE];
	char *title = read_title(title_buffer, stdin);
	void *item;
	if (!title)
	{
		title_read_error();
		return NULL;
	}
	item = OC_find_item_arg(library_title, title, record_title_compare);
	if (!item)
	{
		message_and_error_noflush("No record with that title!\n");
	}
	return item;
}

/* Read in title and get record from library */
struct Record * read_title_get_record(struct Ordered_container *library_title)
{
	return OC_safe_data_ptr(read_title_get_item_ptr(library_title));
}

/* Read record id and get record from library */
struct Record * read_id_get_record(struct Ordered_container *library_id)
{
	int id;
	struct Record *record;
	if (scanf("%d", &id) != 1)
	{
		integer_read_error();
		return NULL;
	}
	record = OC_safe_data_ptr(OC_find_item_arg(library_id, &id, record_id_compare));
	if (!record)
	{
		message_and_error("No record with that ID!\n");
	}
	return record;
}

/* Read in name and get item ptr to collection from library */
void *read_name_get_item_ptr(struct Ordered_container *catalog)
{
	char name[NAME_BUFFER_SIZE];
	void *item;
	if (scanf(NAME_SCAN_BUFFER, name) != 1)
	{
		/* this should never happen because scanf will ignore all whitespace until the next character */
		return NULL;
	}
	item = OC_find_item_arg(catalog, name, collection_name_compare);
	if (!item)
	{
		message_and_error("No collection with that name!\n");
	}
	return item;
}

/* Read in name and get collection from library */
struct Collection * read_name_get_collection(struct Ordered_container *catalog)
{
	return OC_safe_data_ptr(read_name_get_item_ptr(catalog));
}

/* Return non-zero if there are no members, 0 if there are members */
int Collection_not_empty(void* collection_ptr)
{
	return !Collection_empty((const struct Collection *)collection_ptr);
}

/* Used to destroy all records in an Ordered container */
void record_destroy(void * addr)
{
	destroy_Record((struct Record *)addr);
}

/* Used to destroy all collections in an Ordered container */
void collection_destroy(void * addr)
{
	destroy_Collection((struct Collection *)addr);
}

/* Clear catalog */
void clear_catalog(struct Ordered_container *catalog)
{
	OC_apply(catalog, collection_destroy);
	OC_clear(catalog);
}

/* Clear library */
void clear_library(struct Ordered_container *library_title, struct Ordered_container *library_id)
{
	OC_apply(library_title, record_destroy);
	OC_clear(library_title);
	OC_clear(library_id);
	reset_Record_ID_counter();
}

/* Clear all data */
void clear_all(struct Ordered_container *catalog, struct Ordered_container *library_title, struct Ordered_container *library_id)
{
	clear_catalog(catalog);
	clear_library(library_title, library_id);
}

/* Clear all and print a message */
void clear_all_message(struct Ordered_container *catalog, struct Ordered_container *library_title, struct Ordered_container *library_id)
{
	clear_all(catalog, library_title, library_id);
	printf("All data deleted\n");
}

/* Reads in filename and open file with given mode */
FILE * read_filename_open_file(char * mode)
{
	char filename[FILE_BUFFER_SIZE];
	FILE *file;
	if (scanf(FILE_SCAN_BUFFER, filename) != 1)
	{
		file_open_error();
		return NULL;
	}
	file = fopen(filename, mode);
	if (!file)
	{
		file_open_error();
		return NULL;
	}
	return file;
}

/* Flushes the stream until the next \n */
void flush_stream(void)
{
	while (getc(stdin) != '\n');
}

/* Prints message and flushes */
void message_and_error(char * message)
{
	printf(message);
	flush_stream();
}

/* Prints message and flushes */
void message_and_error_noflush(char * message)
{
	printf(message);
}

/* Action Object input error message */
void action_object_input_error(void)
{
	message_and_error("Unrecognized command!\n");
}

/* Title read error message */
void title_read_error(void)
{
	message_and_error_noflush("Could not read a title!\n");
}

/* Integer read error message */
void integer_read_error(void)
{
	message_and_error("Could not read an integer value!\n");
}

/* File open error message */
void file_open_error(void)
{
	message_and_error("Could not open file!\n");
}

/* File invalid error message and closes file */
void file_invalid_error(FILE *file)
{
	message_and_error("Invalid data found in file!\n");
	fclose(file);
}