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

struct token
{
    union
    {
        char ident[MAX_IDENT + 1];
        int  i;
        char c;
    };

    enum toktype kind;
    int          line;
    int          column;
    int          err;
};

struct kwentry
{
    enum toktype kind;
    const char *kw;
};

struct lexer
{
    const unsigned char* src;
    const unsigned char* p;
    int                  line;
    int                  column;
    int                  err;
};

struct lexer lex(const unsigned char *src);

extern struct token tokens[];
extern size_t ntokens;
extern struct kwentry kwtable[];
extern size_t nkw;


#endif
