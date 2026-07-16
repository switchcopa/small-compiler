/* common.c */

#include "common.h"

#include "lexer.h"

void
compiler_internal_abort(const char *file, int line, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, COLOR_BOLD COLOR_RED "internal compiler error:\n");
    fprintf(stderr, "%s: %i: ", file, line);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n" COLOR_RESET);
    va_end(args);

    exit(EXIT_FAILURE);
}

bool
equal(const char *s1, const char *s2)
{
    return (strcmp(s1, s2) == 0);
}

char *
strdup(const char *s)
{
    char *p = malloc(strlen(s) + 1);
    COMPILER_ASSERT(p, OUT_OF_MEMORY_ERR);
    strcpy(p, s);
    return p;
}

static ssize_t
_readfile_helper(const char *filename, char *dest, size_t n)
{
    if (n == 0) return 0;

    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1;
    ssize_t bytes = read(fd, dest, n - 1);
    close(fd);
    
    if (bytes >= 0)
        dest[bytes] = '\0';
    return bytes;
}

struct file *
readfile(const char *filepath)
{
    FILE *fp;
    char *buf;
    ssize_t bytes;
    struct file *file;
    char full_path[PATH_MAX + 1];

    fp = fopen(filepath, "r");
    if (!fp) return NULL;
    file = malloc(sizeof(struct file));
    buf  = malloc(FILE_SIZE);
    COMPILER_ASSERT(file && buf, OUT_OF_MEMORY_ERR);

    if ((bytes = _readfile_helper(filepath, buf, FILE_SIZE)) < 0 ||
         realpath(filepath, full_path) == NULL)
    {
        free(file);
        free(buf);
        return NULL;
    }

    file->filepath = strdup(full_path);
    file->data     = buf;
    file->size     = bytes;
    fclose(fp);
    return file;
} 

void
compiler_vreport_error(struct file *file,
                       enum errtype err,
                       int line,
                       int column,
                       const char *fmt,
                       va_list args)
{
    fprintf(stderr, "small-compiler:\n");
    fprintf(stderr, "%s: ", file->filepath);
    fprintf(stderr, "%i:%i: ", line, column);
    switch (err)
    {
        case ERR_WARNING:
            fprintf(stderr, COLOR_YELLOW);
            break;
        case ERR_ERROR:
            fprintf(stderr, COLOR_RED);
            break;
        case ERR_FATAL:
            fprintf(stderr, COLOR_BOLD COLOR_RED);
            break;
    }
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n" COLOR_RESET);
}

void
compiler_report_error(struct file *file,
                      enum errtype err,
                      int line,
                      int column,
                      const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    compiler_vreport_error(file, err, line, column, fmt, args);
    va_end(args);
}