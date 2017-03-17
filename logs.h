/*
 * logs.h
 */

#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void
print_error_log(
		const char *format,
		...
		);

void
print_warning_log(
		const char *format,
		...
		);

#endif /* LOGS_H_ */
