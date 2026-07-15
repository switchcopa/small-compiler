/* lexer.c */

#include "common.h"

#include "lexer.h"

struct kwentry kwtable[] = 
{
    { KWORD_INT, "int" }
};
size_t nkw = sizeof(kwtable) / sizeof(kwtable[0]);

static inline bool
is_ident_start(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

static inline bool
is_ident_body(char c)
{
    return is_ident_start(c) ||
           (c >= '0' && c <= '9');
}

static inline void
emit(struct lexer *lexer, struct token *t)
{
    if (lexer->ntokens < MAX_TOKENS)
        lexer->tokens[lexer->ntokens++] = *t;
    else
    {
        fprintf(stderr, "too many tokens\n");
        exit(EXIT_FAILURE);
    }
}

static inline char
peek(struct lexer *lexer)
{
    return *lexer->p;
}

static inline char
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
    t->line   = lexer->line;
    t->column = lexer->column;
    t->file   = lexer->file;
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
get_symbol_type(struct lexer *lexer, const char c)
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

static void
skip_whitespace(struct lexer *lexer)
{
    char c;
    while ((c = peek(lexer)) && isspace(c))
    {
        (void)next(lexer);
        if (c == '\n')
        {
            lexer->column = 1;
            lexer->line++;
        }
        else if (c == '\t')
            lexer->column += COLUMN_TAB_INCREMENT(lexer->column) - 1;
    }
}

static void
lex_symbol(struct lexer *lexer)
{
    struct token t;
    char c = peek(lexer);
    t.kind = get_symbol_type(lexer, c);
    t.err = 0;
    if (t.kind == UNKNOWN)
        t.err = lexer->err = true;

    copy_data(lexer, &t);
    emit(lexer, &t);
    (void)next(lexer);
}

static void
lex_ident(struct lexer *lexer)
{
    if (!is_ident_start(peek(lexer)))
        return;

    struct token t;
    copy_data(lexer, &t);
    struct kwentry *p;
    char c;
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
    emit(lexer, &t);
}

static void
lex_num(struct lexer *lexer)
{
    if (!isdigit(peek(lexer)))
        return;

    char num[MAX_IDENT + 1];
    struct token t;
    copy_data(lexer, &t);
    int i = 0;

    while (isdigit(peek(lexer)))
    {
        char c = next(lexer);
        if (i < MAX_IDENT)
            num[i++] = c;
    }

    num[i] = '\0';
    t.i = atoi(num);
    t.err = 0;
    t.kind = INT;
    emit(lexer, &t);
}

struct lexer *
lex(const char *filename)
{
    struct file *file = readfile(filename);
    if (!file) 
    {
        fprintf(stderr, "failed to open or read file '%s'\n", filename);
        return NULL;
    }

    struct lexer *lexer = malloc(sizeof(struct lexer));
    COMPILER_ASSERT(lexer, "fatal! out of memory");

    lexer->ntokens  = 0U;
    lexer->file     = file;
    lexer->src      = lexer->p      = file->data;
    lexer->line     = lexer->column = 1;
    lexer->err      = false;

    char c;
    while ( (c = peek(lexer)) )
    {
        if (is_ident_start(c)) lex_ident(lexer);
        else if (isdigit(c)) lex_num(lexer);
        else if (isspace(c)) skip_whitespace(lexer);
        else lex_symbol(lexer);
    }

    lex_symbol(lexer);
    return lexer;
}