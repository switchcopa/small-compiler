/* parser.h */

#ifndef PARSER_H
#define PARSER_H

#include "common.h"

#include "lexer.h"

#define MAX_PROGRAM_NODES 5120

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

struct program
{
    struct astnode *nodes[MAX_PROGRAM_NODES];
    size_t nnodes;
};

struct parser
{
	struct lexer*   lexer;
	struct file*    file;
	struct token*   tokens;
	struct program* program;
    size_t          ntokens;
    size_t          pos;
	bool		    err;
};

enum precedence
{
    PREC_NONE = 0,
    PREC_ASSIGN,
    PREC_TERM,
    PREC_FACTOR,
    PREC_PRIMARY
};

struct parser  *parse(struct lexer *);
struct program *parse_program(struct parser *);

#endif