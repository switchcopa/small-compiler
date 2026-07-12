/* codegen.c */

#include "common.h"

#include "codegen.h"

struct symtable *
make_symtab(void)
{
    struct symtable *symtab = malloc(sizeof(struct symtable));
    COMPILER_ASSERT(symtab, "fatal! out of memory");

    size_t i;
    for (i = 0; i < SYMBOL_TABLE_ENTRIES; i++)
    {
        symtab->entries[i].name         = NULL;
        symtab->entries[i].stack_offset = 0;
    }

    symtab->nentries          = 0;
    symtab->curr_stack_offset = 0;
    return symtab;
}

int
lookup_sym(struct symtable *symtab, char *name)
{
    int i;
    for (i = 0; i < (int)symtab->nentries; i++)
        if (equal(name, symtab->entries[i].name))
            return i;
    return -1;
}

bool
insert_sym(struct symtable *symtab, char *name, signed int stack_offset)
{
    size_t i;
    for (i = 0; i < symtab->nentries; i++)
        if (symtab->entries[i].name == NULL)
        {
            symtab->entries[i].name         = name;
            symtab->entries[i].stack_offset = stack_offset;
            symtab->curr_stack_offset      -= stack_offset;
            symtab->nentries++;
            return true;
        }
    return false;
}
