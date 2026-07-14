/* codegen.h */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "common.h"

#include "parser.h"

#define SYMBOL_TABLE_ENTRIES 2048

struct sym
{
    char *name;
    struct astnode *expr;
    signed int stack_offset;
    bool is_defined; 
};

struct symtable
{
    struct sym entries[SYMBOL_TABLE_ENTRIES];
    size_t nentries;
    ssize_t curr_stack_offset;
};

struct symtable *init_symtab(void);
int lookup_sym(struct symtable *, char *);
bool insert_sym(struct symtable *, char *, struct astnode *, signed int);

bool analyze_program(struct program *program);

extern struct symtable symtable;

#endif
