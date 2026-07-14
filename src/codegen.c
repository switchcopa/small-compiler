/* codegen.c */

#include "common.h"

#include "codegen.h"

struct symtable symtable;

struct symtable *
init_symtab(void)
{
    size_t i;
    for (i = 0; i < SYMBOL_TABLE_ENTRIES; i++)
    {
        symtable.entries[i].name         = NULL;
        symtable.entries[i].expr         = NULL;
        symtable.entries[i].stack_offset = 0;
        symtable.entries[i].is_defined   = false;
    }

    symtable.nentries          = 0;
    symtable.curr_stack_offset = 0;
    return &symtable;
}

int
lookup_sym(struct symtable *symtab, char *name)
{
    size_t i;
    for (i = 0; i < symtab->nentries; i++)
        if (equal(name, symtab->entries[i].name))
            return i;
    return -1;
}

bool
insert_sym(struct symtable *symtab, char *name, struct astnode *expr, signed int stack_offset)
{
    if (symtab->nentries >= SYMBOL_TABLE_ENTRIES)
        return false;

    size_t i = symtab->nentries;
    symtab->entries[i].name         = name;
    symtab->entries[i].stack_offset = symtab->curr_stack_offset - stack_offset;
    symtab->entries[i].is_defined   = (expr != NULL);
    symtab->entries[i].expr         = expr;

    symtab->curr_stack_offset      -= stack_offset;
    symtab->nentries++;

    return true;
}

static bool
check_expression(struct symtable *symtab, struct astnode *node)
{
    if (!node) return true;

    switch (node->type)
    {
        case NODE_INT_LITERAL:
            return true;
    
        case NODE_IDENT:
            {
                int i;
                if ((i = lookup_sym(symtab, node->as.ident)) < 0)
                {
                    fprintf(stderr, "small-compiler: error: use of undeclared variable '%s'\n",
                            node->as.ident);
                    return false;
                }

                if (!symtab->entries[i].is_defined)
                    fprintf(stderr, "small-compiler: warning: variable '%s' is declared but undefined\n",
                            node->as.ident);
                return true;
            }
        case NODE_ADD:
        case NODE_SUB:
        case NODE_MUL:
        case NODE_DIV:
        case NODE_ASSIGN:
            {
                bool left = check_expression(symtab, node->as.binary.left);
                bool right = check_expression(symtab, node->as.binary.right);
                return left && right;
            }
        case NODE_DECL:
            return true;
    }
    
    return true;
}

bool
analyze_program(struct program *program)
{
    size_t i;
    bool ret = true;
    for (i = 0; i < program->nnodes; i++)
    {
        struct astnode *node = program->nodes[i];
        if (node->type == NODE_DECL)
        {
            // if there is an assignment to the declaration, we recursively check
            // the variables in it if they exist or not
            char *name = node->as.decl.name;
            if (node->as.decl.init)
            {
                if (!check_expression(&symtable, node->as.decl.init))
                    ret = false;
            }

            // we check if it already exists
            // if it doesn't, we see if we can insert it, if there is no empty slot
            // we just end the program
            if (lookup_sym(&symtable, name) < 0)
                COMPILER_ASSERT(insert_sym(&symtable, name, node->as.decl.init, 4), "symtable is full");
            else
            {
                fprintf(stderr, "small-compiler: error: redeclaration of '%s'\n",
                        name);
                ret = false;
            }
            
        }
        else if (node->type == NODE_ASSIGN)
        {
            struct astnode *left= node->as.binary.left;
            int i;
            if ((i = lookup_sym(&symtable, left->as.ident)) < 0)
            {
                fprintf(stderr, "small-compiler: error: use of undefined variable '%s'\n",
                        left->as.ident);
                ret = false;
            }

            if (!check_expression(&symtable, node->as.binary.right))
                ret = false;
            else if (i >= 0)
            {
                symtable.entries[i].is_defined = true;
                symtable.entries[i].expr       = node->as.binary.right;
            }
        }
        else
        {
            if (!check_expression(&symtable, node))
                ret = false;
        }
    }

    return ret;
}
