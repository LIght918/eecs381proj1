#ifndef P1_GLOBALS
#define P1_GLOBALS

#define BUFFER_SIZE 64
#define SCAN_BUFFER "%63s"

extern int g_string_memory;		/* number of bytes used in C-strings */
extern int g_Collection_count;		/* number of collections */
extern int g_Record_count;			/* number of records */

/* These global variables are used to monitor the memory usage of the Ordered_container */
extern int g_Container_count;				/* number of Ordered_containers currently allocated */
extern int g_Container_items_in_use;		/* number of Ordered_container items currently in use */
extern int g_Container_items_allocated;	/* number of Ordered_container items currently allocated */

#endif