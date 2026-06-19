/* lexer_table.c */

#include "lexer.h"

/* this is a GCC/clang extension ONLY */
void (*const lexer_jump_table[256])(struct lexer *) =
{
    [0 ... 255] = lex_symbol,
    [' ']  = skip_whitespace,
    ['\t'] = skip_whitespace,
    ['0'] ... ['9'] = lex_num,
    ['a'] ... ['z'] = lex_ident,
    ['A'] ... ['Z'] = lex_ident,
    ['_'] = lex_ident
};
