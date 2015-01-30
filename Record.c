#include "Record.h"
#include "Utility.h"
#include "p1_globals.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* a Record contains an int ID, rating, and pointers to C-strings for the title and medium */
struct Record {
	char* title;
	int ID;
	char* medium;
	int rating;
};

static int next_record_id;		/* next record id to be assigned */

/* Create a Record object, giving it the next ID number using the ID number counter.
The function that allocates dynamic memory for a Record and the contained data. The rating is set to 0. */
struct Record* create_Record(const char* medium, const char* title)
{
	struct Record *record = malloc(sizeof(struct Record));
	int medium_len = strlen(medium) + 1;
	int title_len = strlen(title) + 1;
	g_string_memory += medium_len + title_len;
	record->medium = strcpy(malloc(medium_len), medium);
	record->title = strcpy(malloc(title_len), title);
	record->rating = 0;
	record->ID = ++next_record_id;
	return record;
}

/* Destroy a Record object
This is the only function that frees the memory for a Record
and the contained data. */
void destroy_Record(struct Record* record_ptr)
{
	free(record_ptr->title);
	free(record_ptr->medium);
	free(record_ptr);
}

/* Accesssors */

/* Return the ID number. */
int get_Record_ID(const struct Record* record_ptr)
{
	return record_ptr->ID;
}

/* Get the title pointer. */
const char* get_Record_title(const struct Record* record_ptr)
{
	return record_ptr->title;
}

/* Set the rating. */
void set_Record_rating(struct Record* record_ptr, int new_rating)
{
	record_ptr->rating = new_rating;
}

/* Print a Record data items to standard output with a final \n character.
Output order is ID number followed by a ':' then medium, rating, title, separated by one space.
If the rating is zero, a 'u' is printed instead of the rating. */
void print_Record(const struct Record* record_ptr)
{
	struct Record record = *record_ptr;
	if (record.rating == 0)
	{
			printf("%d: %s u %s\n", record.ID, record.medium, record.title);
	}
	else
	{
			printf("%d: %s %d %s\n", record.ID, record.medium, record.rating, record.title);
	}
}

/* Write a Record to a file stream with a final \n character.
Output order is ID number, medium, rating, title */
void save_Record(const struct Record* record_ptr, FILE* outfile)
{
	struct Record record = *record_ptr;
	fprintf(outfile, "%d %s %d %s\n", record.ID, record.medium, record.rating, record.title);
}

/* Read a Record's data from a file stream, create the data object and
return a pointer to it, NULL if invalid data discovered in file.
No check made in this function for whether the Record already exists or not.
The counter for the next ID number is set to the largest value found. */
struct Record* load_Record(FILE* infile)
{
	struct Record *record;
	int id, rating;
	char medium[BUFFER_SIZE];
	char title[BUFFER_SIZE];
	char *title_start;
	if (fscanf(infile, "%d", &id) != 1)
	{
		/* ID error */
		return NULL;
	}
	if (fscanf(infile, SCAN_BUFFER, medium) != 1)
	{
		/* medium error */
		return NULL;
	}
	if (fscanf(infile, "%d", &rating) != 1 || rating < 0 || rating > 10)
	{
		/* rating error */
		return NULL;
	}
	title_start = read_title(title, infile);
	if (!title_start)
	{
		/* title error */
		return NULL;
	}
	record = create_Record(medium, title_start);
	record->ID = id;
	if (id > next_record_id)
	{
		next_record_id = id;
	}
	record->rating = rating;
	return record;
}

/* Reset the counter for the next ID number to zero.  */
void reset_Record_ID_counter(void)
{
	next_record_id = 0;
}