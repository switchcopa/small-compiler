/* common.c */

#include "common.h"

#include "lexer.h"

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

char *
strdup(const char *s)
{
    char *p = malloc(strlen(s) + 1);
    COMPILER_ASSERT(p, "fatal! out of memory");
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
readfile(const char *filename)
{
    FILE *fp;
    struct file *file;
    char *buf;
    ssize_t bytes;

    fp = fopen(filename, "r");
    if (!fp) return NULL;
    file = malloc(sizeof(struct file));
    buf  = malloc(FILE_SIZE);
    COMPILER_ASSERT(file && buf, "fatal! out of memory");
    if ((bytes = _readfile_helper(filename, buf, FILE_SIZE)) < 0)
    {
        free(file);
        free(buf);
        return NULL;
    }

    file->filename = strdup(filename);
    file->data     = buf;
    file->size     = bytes;
    fclose(fp);
    return file; 
} 