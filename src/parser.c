/* parser.c */

#include "common.h"

#include "lexer.h"
#include "parser.h"

static struct astnode *astnode_arr[MAX_NODES];
static size_t nnodes;

struct astnode *parse_primary(struct parser *p);
struct astnode *parse_binary(struct parser *p, struct astnode *left);
struct astnode *parse_assignment(struct parser *p, struct astnode *left);
struct astnode *parse_declaration(struct parser *parser);
struct astnode *parse_expression(struct parser *parser, enum precedence binding_power);
struct astnode *parse_statement(struct parser *parser);

static void parser_vreport_error(struct parser *parser, char *msg, va_list args); 
static void parser_report_error(struct parser *parser, char *msg, ...);

struct parser_rule
{
    struct astnode* (*nud)(struct parser *);
    struct astnode* (*led)(struct parser *, struct astnode *);
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

struct astnode*
make_astnode(enum node_type type)
{
    struct astnode *np = malloc(sizeof(struct astnode));
    COMPILER_ASSERT(np, "fatal! out of memory");
    np->type = type;
    COMPILER_ASSERT(nnodes < MAX_NODES, "too many nodes");
    astnode_arr[nnodes++] = np;
    return np;
}

static inline bool
is_type_specifier(enum toktype kind)
{
    switch (kind)
    {
        case KWORD_INT:
        /// ....
            return true;
        default:
            return false;
    }
}

static inline struct token
peek(struct parser *parser)
{
    return parser->tokens[parser->pos];
}

static inline struct token
peek_prev(struct parser *parser)
{
    COMPILER_ASSERT(parser->pos > 0U, "asking for nonexistent previous token");
    return parser->tokens[parser->pos - 1];
}

static inline struct token
advance(struct parser *parser)
{
    COMPILER_ASSERT(parser->pos < parser->ntokens, "can't advance");
    return parser->tokens[parser->pos++];
}

static inline bool
match(struct parser *parser, enum toktype type)
{
    return (peek(parser).kind == type);
}

static void
recover(struct parser *parser)
{
    while (!match(parser, SEMICOLON) &&
           !match(parser, END))
        (void)advance(parser);

    if (match(parser, SEMICOLON))
        (void)advance(parser);
}

static void
parser_vreport_error(struct parser *parser, char *msg, va_list args) 
{
    fprintf(stderr, "small-compiler: ");
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");

    parser->err = 1;
    recover(parser);
}

static void
parser_report_error(struct parser *parser, char *msg, ...) 
{
    va_list args;
    va_start(args, msg);
    parser_vreport_error(parser, msg, args);
    va_end(args);
}

static bool
expect(struct parser *parser, enum toktype kind, char *msg, ...)
{
    if (!match(parser, kind))
    {
        va_list args;
        va_start(args, msg);
        parser_vreport_error(parser, msg, args);
        va_end(args);
        
        return false;
    }

    (void)advance(parser);
    return true;
}

struct astnode *
parse_primary(struct parser *parser)
{
    struct token tok = peek_prev(parser);
    struct astnode *node = NULL;
    switch (tok.kind)
    {
        case INT:
            node = make_astnode(NODE_INT_LITERAL);
            node->as.i = tok.i;
            break;
        case IDENT:
            node = make_astnode(NODE_IDENT);
            node->as.ident = strdup(tok.ident);
            break;
        default:
            COMPILER_ASSERT(0, "unexpected token type in parser");
    }

    return node;
}

struct astnode *
parse_binary(struct parser *parser, struct astnode *left)
{
    struct token tok = peek_prev(parser);
    COMPILER_ASSERT(tok.kind == PLUS   ||
                    tok.kind == MINUS  ||
                    tok.kind == STAR   ||
                    tok.kind == FSLASH,
                    "unexpected token type in parser");
    enum precedence prcd  = rules[tok.kind].precedence;
    struct astnode *right = parse_expression(parser, prcd);
    struct astnode *node  = NULL;

    switch (tok.kind)
    {
        case PLUS:
            node = make_astnode(NODE_ADD);
            break;
        case MINUS:
            node = make_astnode(NODE_SUB);
            break;
        case STAR:
            node = make_astnode(NODE_MUL);
            break;
        case FSLASH:
            node = make_astnode(NODE_DIV);
            break;
        default:
            COMPILER_ASSERT(0, "unexpected token type in parser");
    }

    node->as.binary.left  = left;
    node->as.binary.right = right;
    return node;
}

struct astnode *
parse_assignment(struct parser *parser, struct astnode *left)
{
    if (left->type != NODE_IDENT)
    {
        parser_report_error(parser, "left value required as left operand of assignment");
        return NULL;
    }

    // we allow the nesting of subsequent assignments (a = b = 67) to the right
    // by lowering the precedence to PREC_ASSIGN - 1
    struct astnode *right = parse_expression(parser, PREC_ASSIGN - 1);
    if (right == NULL)
        return NULL;
    struct astnode *node = make_astnode(NODE_ASSIGN);

    node->as.binary.left  = left;
    node->as.binary.right = right;
    return node;
}

struct astnode *
parse_expression(struct parser *parser, enum precedence binding_power)
{
    struct token tok = advance(parser);
    struct parser_rule handler = rules[tok.kind];
    struct astnode *left;

    if (handler.nud == NULL)
    {
        parser_report_error(parser, "%i: %i: Syntax error: expected expression",
                            tok.line, tok.column);
        return NULL;
    }

    left = handler.nud(parser);
    if (!left) return NULL;

    struct token next = peek(parser);
    enum precedence prcd = rules[next.kind].precedence;
    while (binding_power < prcd && rules[next.kind].led)
    {
        next = advance(parser);
        left = rules[next.kind].led(parser, left);

        next = peek(parser);
        prcd = rules[next.kind].precedence;
    }

    return left;
}

struct astnode *
parse_declaration(struct parser *parser)
{
    if (!expect(parser, KWORD_INT, "expected type specifier for declaration"))
        return NULL;

    if (!expect(parser, IDENT, "expected variable identifier after type specifier"))
        return NULL;
    struct token idtok = peek_prev(parser);
    struct astnode *init_expr = NULL;
    struct astnode *np = NULL;

    if (match(parser, EQUALS))
    {
        (void)advance(parser);
        init_expr = parse_expression(parser, PREC_ASSIGN - 1);
        if (init_expr == NULL)
            return NULL;
    }

    if (!expect(parser, SEMICOLON, "expected ';' after statement"))
        return NULL;

    np = make_astnode(NODE_DECL);
    np->as.decl.name   = strdup(idtok.ident);
    np->as.decl.init   = init_expr;
    return np;
}

struct astnode *
parse_statement(struct parser *parser)
{
    if (is_type_specifier(peek(parser).kind))
        return parse_declaration(parser);
    else
    {
        struct astnode *expr = parse_expression(parser, PREC_NONE);
        if (!expect(parser, SEMICOLON, "expected ';' after statement"))
            return NULL;
        return expr;
    }
}

struct program
parse_program(struct parser *parser)
{
    struct program program = {0};

    while (!match(parser, END))
    {
        struct astnode *stmt = parse_statement(parser);
        if (stmt == NULL)
            continue;
    
        COMPILER_ASSERT(program.nnodes < MAX_NODES, "Too many arguments in program");
        program.nodes[program.nnodes++] = stmt;
    }

    return program;
}
