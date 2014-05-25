#include "trace.h"
#include <stdio.h>
#include <string.h>

#define style(c) fprintf(stderr, "\033[%s",c);

// cf : http://www.tux-planet.fr/les-codes-de-couleurs-en-bash/

#define DEFAULT "0;m"

#define BLACK "0;30m"
#define RED "0;31m"
#define GREEN "0;32m"
#define YELLOW "0;33m"
#define BLUE "0;34m"
#define WHITE "0;37m"

#define BOLD "1m"

#define HIGHLIGHT "7m"
#define ERROR "7;30;1;31;1m"

static trace_lvl_t trace_lvl = TRACE_DEBUG;

void set_trace_lvl(trace_lvl_t lvl) {
	trace_lvl = lvl;
}

void print_trace(trace_lvl_t lvl, char *string) {
	if (lvl <= trace_lvl) {
		switch (lvl) {
		case TRACE_ERROR:
			style(ERROR);
			fprintf(stderr, "[ERROR] %s", string);
			style(DEFAULT);
			break;
		case TRACE_STDOUT:
			fprintf(stderr, "%s", string);
			break;
		case TRACE_WARNING:
			style(YELLOW);
			style(BOLD);
			fprintf(stderr, "[WARNING] ");
			style(DEFAULT);
			style(YELLOW);
			fprintf(stderr, "%s", string);
			style(DEFAULT);
			break;
		case TRACE_INFO:
			style(GREEN);
			style(BOLD);
			fprintf(stderr, "[INFO] ");
			style(DEFAULT);	
			style(GREEN);
			fprintf(stderr, "%s", string);
			style(DEFAULT);
			break;
		case TRACE_DEBUG:
			style(WHITE);
			style(BOLD);
			fprintf(stderr, "[DEBUG] ");
			style(DEFAULT);
			style(WHITE);
			fprintf(stderr, "%s", string);
			style(DEFAULT);
			break;
		default:
			fprintf(stderr, "[TRACE_ERROR] : unrecognized trace lvl.\n");
			break;
		}
	}
}
