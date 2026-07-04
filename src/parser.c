/* parser.c */

#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

static struct astnode *astnode_arr[MAX_NODES];
static size_t nnodes;

struct astnode *parse_primary(struct parser *p);
struct astnode *parse_binary(struct parser *p, struct astnode *left);
struct astnode *parse_assignment(struct parser *p, struct astnode *left);

struct parser_rule
{
    struct astnode* (*nud)(struct parser *);
    struct astnode* (*led)(struct parser *, struct astnode *);
    enum precedence precedence;
};

static struct parser_rule rules[] =
{
    [KWORD_INT]     = { NULL,           NULL,             PREC_NONE },
    [IDENT]         = { parse_primary,  NULL,             PREC_NONE },
    [INT]           = { parse_primary,  NULL,             PREC_NONE },
    [EQUALS]        = { NULL,           parse_assignment, PREC_ASSIGN },
    [PLUS]          = { NULL,           parse_binary,     PREC_TERM },
    [MINUS]         = { NULL,           parse_binary,     PREC_TERM },
    [STAR]          = { NULL,           parse_binary,     PREC_FACTOR },
    [FSLASH]        = { NULL,           parse_binary,     PREC_FACTOR },
    [SEMICOLON]     = { NULL,           NULL,             PREC_NONE },
    [END]           = { NULL,           NULL,             PREC_NONE },
    [UNKNOWN]       = { NULL,           NULL,             PREC_NONE },
};

static void
parser_panic(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    for (size_t i = 0; i < nnodes; i++)
        free(astnode_arr[i]);
    exit(EXIT_FAILURE);
}

struct astnode*
make_astnode(enum node_type type)
{
    struct astnode *np = malloc(sizeof(struct astnode));
    if (!np) parser_panic("fatal! out of memory");
    np->type = type;

    if (nnodes >= MAX_NODES)
        parser_panic("too many nodes");
    else
        astnode_arr[nnodes++] = np;
    return np;
}

static inline struct token
peek(struct parser *parser)
{
    return parser->tokens[parser->pos];
}

static inline struct token
advance(struct parser *parser)
{
    if (parser->pos >= parser->ntokens)
        parser_panic("can't advance to next token");
    return parser->tokens[parser->pos++];
}

static inline bool
match(struct parser *parser, enum toktype type)
{
    return (peek(parser).kind == type);
}

static void
recover(struct parser *parser)
{
    while (peek(parser).kind != SEMICOLON &&
            peek(parser).kind != END)
        (void)advance(parser);
}

static void
parser_report_error(struct parser *parser, char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    fprintf(stderr, "small-compiler: ");
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");

    parser->err = 1;
    recover(parser);
}

struct astnode *
parse_expression(struct parser *parser, enum precedence binding_power)
{
    struct token tok = advance(parser);
    struct parser_rule handler = rules[tok.kind];
    struct astnode *left;

    if (handler.nud == NULL)
    {
        parser_report_error(parser, "%i: %i: Syntax error: expected expression",
                            tok.line, tok.column);
        return NULL;
    }

    left = handler.nud(parser);
    struct token next = peek(parser);
    enum precedence prcd = rules[next.kind].precedence;
    while (binding_power < prcd && rules[next.kind].led)
    {
        next = advance(parser);
        left = rules[next.kind].led(parser, left);

        next = peek(parser);
        prcd = rules[next.kind].precedence;
    }

    return left;
}
