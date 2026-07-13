
/* this main.c is just temporary,
 * I'm testing the lexer, the parser,
 * etc...
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexer.h"
#include "parser.h"
#include "codegen.h"

#define MAX_LINE 256

static const char *token_names[] = {
    [KWORD_INT] = "KWORD_INT",
    [IDENT]     = "IDENT",
    [INT]       = "INT",
    [EQUALS]    = "EQUALS",
    [PLUS]      = "PLUS",
    [MINUS]     = "MINUS",
    [STAR]      = "STAR",
    [FSLASH]    = "FSLASH",
    [SEMICOLON] = "SEMICOLON",
    [END]       = "END",
    [UNKNOWN]   = "UNKNOWN"
};

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
    struct lexer l = lex(buf);
    if (l.err)
    {
        fprintf(stderr, "lexical errors encountered. aborting\n");
        free(buf);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < ntokens; i++)
        printf("%s ", token_names[(int)tokens[i].kind]);
    printf("\n");
    struct parser p;
    p.tokens = tokens;
    p.ntokens = ntokens;
    p.pos = p.err = 0;

    struct program program = parse_program(&p);
    if (p.err)
    {
        fprintf(stderr, "parsing errors occurred. aborting\n");
        free(buf);
        return EXIT_FAILURE;
    }

    // 5. Semantic Analysis (Our symbol table and variable check!)
    init_symtab();
    if (!analyze_program(&program))
    {
        fprintf(stderr, "Semantic errors encountered. Aborting code generation.\n");
        free(buf);
        return EXIT_FAILURE;
    }

    printf("\n\033[32m\033[1mSuccess! program parsed and analyzed nicely :D!\033[0m\n\n");
    free(buf);
    return EXIT_SUCCESS;
}
