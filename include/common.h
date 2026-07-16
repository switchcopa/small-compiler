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
#include <fcntl.h>
#include <unistd.h>

/* global limits and config */
#define MAX_IDENT           32
#define MAX_TOKENS          4096
#define TAB_WIDTH           4
#define MAX_NODES           2048
#define FILE_SIZE           4096U
#define OUT_OF_MEMORY_ERR   "fatal! out of memory"

/* colors for output */
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RESET   "\033[0m"

/* global structs */
struct file
{
    char*    filepath;
    char*    data;
    size_t   size;
};

/* global enums */
enum errtype
{
    ERR_WARNING = 0,
    ERR_ERROR,
    ERR_FATAL
};

/* global util macros */
#define COLUMN_TAB_INCREMENT(c) \
        (TAB_WIDTH - (((c) - 1) % TAB_WIDTH))

/* global util functions */
bool            equal(const char *, const char *);
char*           strdup(const char *);
struct file*    readfile(const char *);

/* error reporting functions */
void compiler_report_error(struct file *, enum errtype, int, int, const char *, ...);
void compiler_vreport_error(struct file *, enum errtype, int, int, const char *, va_list); 

/* internal compiler assertions */
void compiler_internal_abort(const char *file, int line, const char *fmt, ...);

#define COMPILER_ASSERT(condition, msg)                         \
    do {                                                        \
        if (!(condition)) {                                     \
            compiler_internal_abort(__FILE__, __LINE__, msg);   \
        }                                                       \
    } while (0)

#endif