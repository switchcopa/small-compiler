
/* this main.c is just temporary,
 * I'm testing the lexer, the parser,
 * etc...
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "lexer.h"
#include "types.h"

#define MAX_LINE 256

static void error(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "small-compiler: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(EXIT_FAILURE);
}

static unsigned char *readfile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    if (size < 0)
    {
        fclose(fp);
        error("failure while reading the size of %s", filename);
    }

    unsigned char *buf = malloc(size + 1);
    if (!buf)
    {
        fclose(fp);
        error("fatal: out of memory");
    }

    size_t bytes = fread(buf, 1, size, fp);
    buf[bytes] = '\0';
    return buf;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: ./build/small-compiler test.c\n");
        exit(EXIT_FAILURE);
    }

    unsigned char *buf = readfile(argv[1]);
    (void)lex(buf);

    for (size_t i = 0; i < ntokens; i++)
    {
        char *p = str_toktypes[(int)tokens[i].kind];
        fprintf(stdout, "%s ", p);
    }
    fprintf(stdout, "\n");
    return EXIT_SUCCESS;
}
