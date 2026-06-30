/* parser.c */


#include "lexer.h"
#include "parser.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

static struct astnode *astnode_arr[MAX_NODES];
static size_t nnodes;

struct astnode *parse_primary(struct parser *p);
struct astnode *parse_binary(struct parser *p);
struct astnode *parse_assignment(struct parser *p);

struct parser_rule
{
    struct astnode* (*nud)(struct parser *);
    struct astnode* (*led)(struct parser *, struct astnode *left);
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
free_astnodes(void)
{
    for (size_t i = 0; i < nnodes; i++)
        free(astnode_arr[i]);
}

static void
parser_report_error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    free_astnodes();
    exit(EXIT_FAILURE);
}

static struct astnode*
make_astnode(enum node_type type)
{
    struct astnode *np = malloc(sizeof(astnode));
    if (!np) parser_report_error("fatal! out of memory");
    np->type = type;

    if (nnodes >= MAX_NODES)
        parser_report_error("too many nodes");
    else
        astnode_arr[nnodes++] = np;
    return np;
}
