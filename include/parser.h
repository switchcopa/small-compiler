/* parser.h */

#ifndef PARSER_H
#define PARSER_H

#include "common.h"

#include "lexer.h"

enum node_type
{
	NODE_INT_LITERAL = 0,
	NODE_IDENT,
	NODE_ASSIGN,
	NODE_ADD,
	NODE_SUB,
	NODE_MUL,
	NODE_DIV,
    NODE_DECL
};

struct astnode
{
	enum node_type type;
	union
	{
		int i;
		char *ident;

		struct
		{
			struct astnode *left;
			struct astnode *right;
		} binary;

        struct
        {
            char *name;
            struct astnode *init;
        } decl;
	} as;
};

struct parser
{
	struct token* tokens;
    size_t        ntokens;
    size_t        pos;
	int           err;
};

enum precedence
{
    PREC_NONE = 0,
    PREC_ASSIGN,
    PREC_TERM,
    PREC_FACTOR,
    PREC_PRIMARY
};

struct astnode *parse_expression(struct parser *, enum precedence);
void parse_program(struct parser *p);

#endif
