/*
 * utils.c
 */

#include "utils.h"

/* caller should free the new allocated string to avoid memory leak */
char* duplicate_string(const char *s)
{
	char *s2 = (char*)calloc(strlen(s), sizeof(char));

	if (!s2)
	{
		/* out of memory, don't continue */
		return NULL;
	}

	strncpy(s2, s, strlen(s));

	return s2;
}



