#ifndef UTILITY_H
#define UTILITY_H

#include "Record.h"
#include "p1_globals.h"
#include <stdio.h>

#define TITLE_BUFFER_SIZE 64
#define TITLE_SCAN_BUFFER "%63s"
#define TITLE_BUFFER_END 62
#define NAME_BUFFER_SIZE 16
#define NAME_SCAN_BUFFER "%15s"
#define MEDIUM_BUFFER_SIZE 8
#define MEDIUM_SCAN_BUFFER "%7s"
#define FILE_BUFFER_SIZE 32
#define FILE_SCAN_BUFFER "%31s"
#define RATING_MAX 5
#define RATING_MIN 1

/* Print a record */
void record_print(void* record);

/* Compare records by their titles */
int record_compare_title(const void* first_record, const void* second_record);

/* Compare records by their ids */
int record_compare_id(const void* first_record, const void* second_record);

/* Compares a record's title with the given title */
int record_title_compare(const void* title, const void* record);

/* Compares a record's id with the given id */
int record_id_compare(const void* title, const void* record);

/* Read in a title from the specified file, returns 0 on success and nonzero on failure */
char * read_title(char *title, FILE *infile);

#endif
