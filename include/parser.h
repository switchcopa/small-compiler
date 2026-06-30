/* parser.h */

#ifndef PARSER_H
#define PARSER_H

// #define 

#include "lexer.h"

enum node_type
{
	// Literals
	NODE_INT_LITERAL = 0,
	NODE_IDENT,

	// BINARY MATH OPERATORS
	NODE_ASSIGN,
	NODE_ADD,
	NODE_SUB,
	NODE_MUL,
	NODE_DIV,
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
	} as;
};

struct parser
{
	struct token* tokens;
	struct token* p;
	int           err;
};

extern struct astnode *tree;

void parse_program(struct parser *p);

#endif
