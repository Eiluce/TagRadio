#ifndef __TRACE__
#define __TRACE__

typedef enum {
	TRACE_ERROR = 0,
	TRACE_STDOUT = 1,
	TRACE_WARNING = 2,
	TRACE_INFO = 3,
	TRACE_DEBUG = 4
} trace_lvl_t;

extern void set_trace_lvl(trace_lvl_t lvl);
	
extern void print_trace(trace_lvl_t lvl, char *string);

#endif
