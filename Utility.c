#include "Utility.h"
#include "Record.h"
#include "p1_globals.h"

/* Compare records by their titles */
int record_compare_title(const struct Record *first_record, const struct Record *second_record)
{
	return strcomp(get_Record_title(first_record), get_Record_title(second_record));
}

/* Compare records by their ids */
int record_compare_id(const struct Record *first_record, const struct Record *second_record)
{
	return get_Record_ID(first_record) - get_Record_ID(second_record);
}

/* Compares a record's title with the given title */
int record_title_compare(const char *title, const struct Record *record)
{
	return strcomp(title, get_Record_title(record));
}

/* Compares a record's id with the given id */
int record_id_compare(const int * id, const struct Record *record)
{
	return *id - get_Record_ID(record);
}

/* Read in a title from the specified file, returns pointer to the title on success and a NULL on failure */
char * read_title(char *title, FILE *infile)
{
	char *title_start = title;
	char *title_end = title + SCAN_BUFFER_SIZE - 1;
	/* note: title + SCAN_BUFFER_SIZE is the last character, which must be \0...we want the character before*/
	if (!fgets(title, BUFFER_SIZE, infile))
	{
		/* title read error */
		return NULL;
	}
	/* find the first block of whitespace characters */
	while (title_start < title_end && !isspace((int)*(title_start++))) {}
	/* find the end of the first whitespace block */
	while (title_start < title_end && isspace((int)*(title_start++))) {}
	/* find the beginning of the terminating whitespace */
	while (title_start < title_end && isspace((int)*(title_end--))) {}
	if (title_start == title_end)
	{
		/* title read error */
		return NULL;
	}
	/* set the first character of the terminating whitespace to \0 */
	*(title_end + 1) = '\0';
	return --title_start;
}