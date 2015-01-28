#include "p1_globals.h"

int g_string_memory = 0;		/* number of bytes used in C-strings */
int g_Collection_count = 0;		/* number of collections */
int g_Record_count = 0;			/* number of records */

								/* These global variables are used to monitor the memory usage of the Ordered_container */
int g_Container_count = 0;				/* number of Ordered_containers currently allocated */
int g_Container_items_in_use = 0;		/* number of Ordered_container items currently in use */
int g_Container_items_allocated = 0;	/* number of Ordered_container items currently allocated */