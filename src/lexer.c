/* lexer.c */

#include "lexer.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

struct token tokens[MAX_TOKENS];
size_t ntokens;
struct kwentry kwtable[] = 
{
    { KWORD_INT, "int" }
};
size_t nkw = sizeof(kwtable) / sizeof(kwtable[0]);

static inline bool
is_ident_start(unsigned char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

static inline bool
is_ident_body(unsigned char c)
{
    return is_ident_start(c) || isdigit(c);
}

static inline void
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

static inline unsigned char
peek(struct lexer *lexer)
{
    return *lexer->p;
}

static inline unsigned char
next(struct lexer *lexer)
{
    if (peek(lexer))
    {
        lexer->column++;
        return *lexer->p++;
    } else
        return '\0';
}

static inline void
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
            lexer->line, lexer->column, c);
            return UNKNOWN;
    }
}

void
skip_whitespace(struct lexer *lexer)
{
    for (unsigned char c = next(lexer);
            c && isspace(c); c = next(lexer))
    {
        if (c == '\n')
        {
            lexer->column = 1;
            lexer->line++;
        }
        else if (c == '\t')
            lexer->column += COLUMN_TAB_INCREMENT(lexer->column);
    }
}

void
lex_symbol(struct lexer *lexer)
{
    struct token t;
    unsigned char c = peek(lexer);
    t.kind = get_symbol_type(lexer, c);
    t.err = 0;
    if (t.kind == UNKNOWN)
        t.err = lexer->err = 1;
    copy_data(lexer, &t);
    emit(t);
    (void)next(lexer);
}

void
lex_ident(struct lexer *lexer)
{
    if (!is_ident_start(peek(lexer)))
        return;

    struct token t;
    struct kwentry *p;
    unsigned char c;
    int i = 0;

    while ((c = peek(lexer)) && is_ident_body(c))
    {
        if (i < MAX_IDENT)
            t.ident[i++] = c;
        (void)next(lexer);
    }

    t.ident[i] = '\0';
    t.err = 0;
    t.kind = ( (p = get_kwentry(t.ident)) ) 
                ? p->kind : IDENT;
    copy_data(lexer, &t);
    emit(t);
}

void
lex_num(struct lexer *lexer)
{
    if (!isdigit(peek(lexer)))
        return;

    char num[MAX_IDENT + 1];
    struct token t;
    int i = 0;

    while (isdigit(peek(lexer)))
    {
        unsigned char c = next(lexer);
        if (i < MAX_IDENT)
            num[i++] = c;
    }

    num[i] = '\0';
    t.i = atoi(num);
    t.err = 0;
    t.kind = INT;
    copy_data(lexer, &t);
    emit(t);
}

struct lexer
lex(const unsigned char *src)
{
    struct lexer lexer;

    lexer.src  = lexer.p      = src;
    lexer.line = lexer.column = 1;
    lexer.err  = 0;

    unsigned char c;    
    while ( (c = peek(&lexer)) )
        lexer_jump_table[c](&lexer);

    lex_symbol(&lexer);    
    return lexer;
}
