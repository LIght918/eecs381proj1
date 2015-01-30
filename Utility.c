#include "Utility.h"
#include "Record.h"
#include <ctype.h>
#include <string.h>
#include "p1_globals.h"

#include <stdio.h>

/* Print a record */
void record_print(void* record)
{
	print_Record((struct Record *)record);
}

/* Compare records by their titles */
int record_compare_title(const void* first_record, const void* second_record)
{
	printf("rec com title\n");
	return strcmp(get_Record_title((const struct Record *)first_record), get_Record_title((const struct Record *)second_record));
}

/* Compare records by their ids */
int record_compare_id(const void* first_record, const void* second_record)
{
	printf("rec com id\n");
	return get_Record_ID((const struct Record *)first_record) - get_Record_ID((const struct Record *)second_record);
}

/* Compares a record's title with the given title */
int record_title_compare(const void* title, const void* record)
{
	printf("rec title com\n");
	return strcmp((const char*)title, get_Record_title((const struct Record *)record));
}

/* Compares a record's id with the given id */
int record_id_compare(const void* id, const void* record)
{
	printf("rec id com\n");
	return *((int *)id) - get_Record_ID((const struct Record *)record);
}

/* Read in a title from the specified file, returns pointer to the title on success and a NULL on failure */
char * read_title(char *title, FILE *infile)
{
	int i;
	int index = 0;
	int last_was_whitespace = 1;
	int last_char_index = -1;
	/* note: title + SCAN_BUFFER_SIZE is the last character, which must be \0...we want the character before*/
	if (!fgets(title, BUFFER_SIZE, infile))
	{
		/* title read error */
		return NULL;
	}
	for (i = 0; i < strlen(title); i++)
	{
		char current = *(title + i);
		int is_whitespace = isspace(current);
		if (!last_was_whitespace && is_whitespace)
		{
			*(title + index++) = ' ';
			last_was_whitespace = 1;
		}
		else if (!is_whitespace)
		{
			last_char_index = index;
			*(title + index++) = current;
			last_was_whitespace = 0;
		}
	}
	if (last_char_index == -1)
	{
		/* title read error */
		return NULL;
	}
	*(title + last_char_index + 1) = '\0';
	return title;
}