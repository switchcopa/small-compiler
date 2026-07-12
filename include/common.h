/* common.h */

#ifndef COMMON_H
#define COMMON_H

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>

/* global limits and config */
#define MAX_IDENT           32
#define MAX_TOKENS          4096
#define TAB_WIDTH           4
#define MAX_NODES           2048

/* global util macros */
#define COLUMN_TAB_INCREMENT(c) \
        (TAB_WIDTH - (((c) - 1) % TAB_WIDTH))

/* global util functions */
bool equal(const char *, const char *);

/* internal compiler assertions */
void compiler_internal_abort(const char *file, int line, const char *fmt, ...);

#define COMPILER_ASSERT(condition, msg)                         \
    do {                                                        \
        if (!(condition)) {                                     \
            compiler_internal_abort(__FILE__, __LINE__, msg);   \
        }                                                       \
    } while (0)

#endif
