/* lexer.h */

#ifndef LEXER_H
#define LEXER_H

#define MAX_IDENT 32
#define MAX_TOKENS 4096

#include <stddef.h>

extern struct token tokens[];
extern size_t ntokens;
extern struct kwentry kwtable[];
extern size_t nkw;

enum toktype
{
    KWORD_INT,
    IDENT,
    INT,
    EQUALS,
    PLUS,
    MINUS,
    STAR,
    FSLASH,
    SEMICOLON,
    NULL,
    UNKNOWN
};

struct token
{   
    union
    {
        char ident[MAX_IDENT + 1];
        int i;
        char c;
    };

    enum toktype kind;
    int line;
    int column;
    int err;
};

struct kwentry
{
    enum toktype kind;
    const char *kw;
};

struct lexer
{
    const char *src;
    const char *p;
    int line;
    int column;
};

struct lexer lex(const char *src); // questionable?

#endif