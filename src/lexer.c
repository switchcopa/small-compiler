/* lexer.c */

#include "lexer.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct token tokens[MAX_TOKENS];
size_t ntokens;
struct kwentry kwtable[] = 
{
    { KWORD_INT, "int" }
};
size_t nkw = sizeof(kwtable)/sizeof(kwtable[0]);

inline static void
emit(struct token t)
{
    if (ntokens < MAX_TOKENS)
        tokens[ntokens++] = t;
    else
    {
        fprintf(stderr, "too many tokens\n");
        exit(EXIT_FAILURE);
    }
}

inline static unsigned char
peek(struct lexer *lexer)
{
    return (unsigned char)*lexer->p;
}

/* should add column modifications later*/
inline static unsigned char
next(struct lexer *lexer)
{
    return (peek(lexer)) ? (unsigned char)*lexer->p++ : '\0';
}

/*
inline static void
back(struct lexer *lexer)
{
    lexer->p--;
}

inline static struct token
make_token(enum toktype kind, int line, int err)
{
    struct token t;

    t.kind = kind;
    t.line = line;
    t.err = err;

    return t;
}

inline static void
skip_whitespace(struct lexer *lexer)
{
    while (peek(lexer) && isspace(*lexer->p))
        next(lexer);
}

inline static void
recover(struct lexer *lexer)
{
    unsigned char c;
    while ((c = peek(lexer)) != ';' && c)
        c = next(lexer);
}
*/

inline static void
copy_data(struct lexer *lexer, struct token *t)
{
    t->line = lexer->line;
    t->column = lexer->column;
}

static struct kwentry *
get_kwentry(const char *kw)
{
    struct kwentry *p;
    for (p = kwtable; p < kwtable + nkw; p++)
        if (strcmp(p->kw, kw) == 0)
            return p;

    return NULL;
}

static enum toktype
get_symbol_type(struct lexer *lexer, const unsigned char c)
{
    switch(c)
    {
        case '=':
            return EQUALS;
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '*':
            return STAR;
        case '/':
            return FSLASH;
        case ';':
            return SEMICOLON;
        case '\0':
            return END;
        default:
            fprintf(stderr, "%i:%i: what is this token: %c\n",
            lexer->line, lexer->column);
            return UNKNOWN;
    }
}

static void
lex_ident(struct lexer *lexer)
{
    if (!isalpha(peek(lexer)))
        return;

    struct token t;
    struct kwentry *p;
    unsigned char c;
    int i = 0;

    while ((c = next(lexer)) && (isalnum(c) || c == '_')) // major bug in next()
    {
        if (i < MAX_IDENT)
            t.ident[i++] = c;
        else continue; // just ignore large identifiers without buffer overflowing
    }

    t.ident[i] = '\0';
    t.err = 0;
    t.kind = ((p = get_kwentry(t.ident))) ? p->kind : IDENT;
    copy_data(lexer, &t);
    emit(t);
}

static void
lex_num(struct lexer *lexer)
{
    if (!isdigit(peek(lexer)))
        return;

    char num[MAX_IDENT + 1];
    unsigned char c;
    struct token t;
    int i = 0;

    while (isdigit( c = next(lexer) )) // major bug here too
    {
        if (i < MAX_IDENT)
            num[i++] = c;
        else continue;
    }

    num[i] = '\0';
    t.i = atoi(num);
    t.err = 0;
    t.kind = INT;
    copy_data(lexer, &t);
    emit(t);
}

/*  task: write the proper skip_whitespace to
    skip all spaces and tabs, while updating
    the lexer line and column

    this function should loop through the 
    characters of the source code, if the char
    is a space, we call skip_whitespace, if it's
    a character, we call lex_ident, if it's a num,
    we call lex_num, and if not all of this,
    we call get_symbol_type

    in the end, we finish with an end token.
    we should have the tokens[] array, to be
    passed to the parser.
    */
struct lexer
lex(const char *src)
{
    struct lexer lexer;

    lexer.src = lexer.p = src;
    lexer.line = lexer.column = 1;

    /* while (process the next character and decide
                which function to call for it to build
                the token array)
    */
    return lexer;
}