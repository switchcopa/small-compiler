/* lexer.h */

#ifndef LEXER_H
#define LEXER_H

#define MAX_IDENT       32
#define MAX_TOKENS      4096
#define TAB_WIDTH       4

#define COLUMN_TAB_INCREMENT(c) \
        (TAB_WIDTH - (((c) - 1) % TAB_WIDTH))

#include <stddef.h>

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
