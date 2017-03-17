/*
 * logs.c
 */

#include "logs.h"

#define WARNING "[MyAssembler Warning]: "
#define ERROR   "[MyAssembler Error]: "

void
print_error_log(
		const char *format,
		...
		)
{
	va_list args;

	va_start(args, format);
	vfprintf(
			stdout,
			format,
			args);
	va_end(args);
}

void
print_warning_log(
		const char *format,
		...
		)
{
	va_list args;

	va_start(args, format);
	vfprintf(
			stdout,
			format,
			args);
	va_end(args);
}
