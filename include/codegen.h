/* codegen.h */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "common.h"

#define SYMBOL_TABLE_ENTRIES 2048

struct sym
{
    char *name;
    signed int stack_offset;
};

struct symtable
{
    struct sym entries[SYMBOL_TABLE_ENTRIES];
    size_t nentries;
    ssize_t curr_stack_offset;
};

struct symtable *make_symtab(void);
int lookup_sym(struct symtable *, char *);
bool insert_sym(struct symtable *, char *, signed int);

#endif
