/* lexer.h */

#ifndef LEXER_H
#define LEXER_H

#include "common.h"

enum toktype
{
    KWORD_INT = 0,
    IDENT,
    INT,
    EQUALS,
    PLUS,
    MINUS,
    STAR,
    FSLASH,
    SEMICOLON,
    END,
    UNKNOWN
};

struct file
{
    char    *filename;
    char    *data;
    size_t   size;
    int      fd;
};

struct token
{
    union
    {
        char ident[MAX_IDENT + 1];
        int  i;
        char c;
    };

    struct file* file;
    enum toktype kind;
    int          line;
    int          column;
    bool         err;
};

struct kwentry
{
    enum toktype kind;
    const char *kw;
};

struct lexer
{
    struct token         tokens[MAX_TOKENS];
    size_t               ntokens;
    struct file*         file;
    const char*          src;
    const char*          p;
    int                  line;
    int                  column;
    bool                 err;
};

struct lexer *lex(const char *);

extern struct kwentry kwtable[];
extern size_t nkw;

#endif
