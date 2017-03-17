/*
 * logs.h
 */

#ifndef LOGS_H_
#define LOGS_H_

#include "arch.h"

#define WARNING "[MyAssembler Warning]:"
#define ERROR   "[MyAssembler Error]:"

void
print_log(
		const char *format,
		...
		);

#endif /* LOGS_H_ */
