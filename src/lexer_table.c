/* lexer_table.c */

#include "lexer.h"

/* this is a GCC/clang extension ONLY */
void (*lexer_jump_table[256])(struct lexer *);

__attribute__((constructor))
static void init_lexer_table(void)
{
    for (int i = 0; i < 256; i++)
        lexer_jump_table[i] = lex_symbol;

    lexer_jump_table[' '] = skip_whitespace;
    lexer_jump_table['\t'] = skip_whitespace;
    lexer_jump_table['_'] = lex_ident;

    for (int i = '0'; i <= '9'; i++) lexer_jump_table[i] = lex_num;
    for (int i = 'a'; i <= 'z'; i++) lexer_jump_table[i] = lex_ident;
    for (int i = 'A'; i <= 'Z'; i++) lexer_jump_table[i] = lex_ident;
}
