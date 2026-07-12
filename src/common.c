/* common.c */

#include "common.h"

void
compiler_internal_abort(const char *file, int line, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "internal compiler error:\n");
    fprintf(stderr, "%s: %i: ", file, line);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(EXIT_FAILURE);
}

bool
equal(const char *s1, const char *s2)
{
    return (strcmp(s1, s2) == 0);
}
