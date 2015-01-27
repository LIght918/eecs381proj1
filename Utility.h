#ifndef UTILITY_H
#define UTILITY_H

#include "Record.h"
#include "p1_globals.h"
#include <stdio.h>

/* Compare records by their titles */
int record_compare_title(const struct Record *first_record, const struct Record *second_record);

/* Compare records by their ids */
int record_compare_id(const struct Record *first_record, const struct Record *second_record);

/* Compares a record's title with the given title */
int record_title_compare(const char *title, const struct Record *record);

/* Compares a record's id with the given id */
int record_id_compare(const char *title, const struct Record *record);

/* Read in a title from the specified file, returns 0 on success and nonzero on failure */
char * read_title(char *title, FILE *infile);

#endif
