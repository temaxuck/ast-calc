#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    AST_NODE_NUM, AST_NODE_OP
} ASTNodeKind;

typedef enum {
    AST_OP_PLUS,
    AST_OP_MINUS,
    AST_OP_MULTIPLY,
    AST_OP_DIVIDE,
} ASTOp;

typedef struct ASTNode {
    ASTNodeKind t;
    union {
        double value;
        struct {
            ASTOp op;
            struct ASTNode *lvalue;
            struct ASTNode *rvalue;
        } expr;
    };
} ASTNode;

ASTNode *ast_number(double value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->t = AST_NODE_NUM;
    node->value = value;
    return node;
}

ASTNode *ast_op(ASTOp op, ASTNode *lvalue, ASTNode *rvalue) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->t = AST_NODE_OP;
    node->expr.op = op;
    node->expr.lvalue = lvalue;
    node->expr.rvalue = rvalue;
    return node;
}

double ast_handle_op(double lvalue, double rvalue, ASTOp op) {
    switch (op) {
    case AST_OP_PLUS:
        return lvalue + rvalue;
    case AST_OP_MINUS:
        return lvalue - rvalue;
    case AST_OP_MULTIPLY:
        return lvalue * rvalue;
    case AST_OP_DIVIDE:
        if (rvalue == 0) {
            printf("Cannot divide by 0");
            exit(2);
        }
        return lvalue / rvalue;
    default:
        assert(false && "Unreachable");
    }
}

double ast_eval(ASTNode *root) {
    switch (root->t) {
    case AST_NODE_NUM: return root->value;
    case AST_NODE_OP: return ast_handle_op(ast_eval(root->expr.lvalue), ast_eval(root->expr.rvalue), root->expr.op);
    default:
        assert(false && "Unreachable");
    }
}

void ast_free_node(ASTNode *root) {
    if (root == NULL) return;
    if (root->expr->lvalue != NULL) ast_free_node(root->expr->lvalue);
    if (root->expr->rvalue != NULL) ast_free_node(root->expr->rvalue);
}

#ifdef ASTPARSER
#include <string.h>
#include "astlexer.h"

#define PUNCTS_COUNT 6
const char *puncts[PUNCTS_COUNT] = {"+", "-", "*", "/", "(", ")"};

typedef struct {
    const char *msg;
    size_t msg_len;
    size_t loc;
} ASTParserError;

ASTParserError _err = {0};

ASTParserError astparser_get_error() {
    return _err;
}

void astparser_report_error(const char *msg, size_t loc) {
    _err.msg = msg;
    _err.msg_len = strlen(msg);
    _err.loc = loc;
}

ASTNode *astparser_factor(ASTLexer *l, ASTLexerToken *t);
ASTNode *astparser_term(ASTLexer *l, ASTLexerToken *t);
ASTNode *astparser_expr(ASTLexer *l, ASTLexerToken *t);

ASTNode *astparser_factor(ASTLexer *l, ASTLexerToken *t) {
    if (!astlexer_get_token(l, t)) {
        astparser_report_error("Unexpected end of input", t->loc);
        return NULL;
    }
    if (t->t == ASTLEXER_NUMBER) return ast_number(t->numval);
    if (t->t == ASTLEXER_PUNCT && astlexer_token_cmp_cstr(t, "(")) {
        ASTNode *node = astparser_expr(l, t);
        if (!node) return NULL;
        if (!astlexer_get_token(l, t)) { // unexpected EOI
            astparser_report_error("Unexpected end of input", t->loc);
            return NULL;
        }
        if (!(t->t == ASTLEXER_PUNCT && astlexer_token_cmp_cstr(t, ")"))) { // expected ')'
            astparser_report_error("Expected ')'", t->loc);
            return NULL;
        }
        return node;
    }

    astparser_report_error("NAN", t->loc);
    return NULL;
}

ASTNode *astparser_term(ASTLexer *l, ASTLexerToken *t) {
    ASTNode *root = astparser_factor(l, t);
    if (!root) {
        astparser_report_error("Expected number or expression", t->loc);
        return NULL;
    }

    while (true) {
        if (astlexer_peek(l, t) && t->t != ASTLEXER_PUNCT) break;
        ASTNode *op_node;
        if (astlexer_token_cmp_cstr(t, "*"))
            op_node = ast_op(AST_OP_MULTIPLY, root, NULL); // right value to be added later
        else if (astlexer_token_cmp_cstr(t, "/"))
            op_node = ast_op(AST_OP_DIVIDE, root, NULL); // right value to be added later
        else break;
        
        astlexer_get_token(l, t); // consume the peeked token
        ASTNode *rvalue = astparser_factor(l, t);
        if (!rvalue) {
            ast_free_node(op_node);
            astparser_report_error("Expected number or expression", t->loc);
            return NULL;
        }
        op_node->expr.rvalue = rvalue;
        root = op_node;
    }
    
    return root;
}

ASTNode *astparser_expr(ASTLexer *l, ASTLexerToken *t) {
    if (!astlexer_peek(l, t) && t->t == ASTLEXER_EOI) return ast_number(.0f);
    ASTNode *root = astparser_term(l, t);
    if (!root) {
        astparser_report_error("Expected number or expression", t->loc);
        return NULL;
    }

    while (true) {
        if (astlexer_peek(l, t) && t->t != ASTLEXER_PUNCT) break;
        ASTNode *op_node;
        if (astlexer_token_cmp_cstr(t, "+"))
            op_node = ast_op(AST_OP_PLUS, root, NULL); // right value to be added later
        else if (astlexer_token_cmp_cstr(t, "-"))
            op_node = ast_op(AST_OP_MINUS, root, NULL); // right value to be added later
        else break;
        
        astlexer_get_token(l, t); // consume the peeked token
        ASTNode *rvalue = astparser_term(l, t);
        if (!rvalue) {
            ast_free_node(op_node);
            astparser_report_error("Expected number or expression", t->loc);
            return NULL;
        }
        op_node->expr.rvalue = rvalue;
        root = op_node;
    }
    
    return root;
}

ASTNode *astparser_parse(const char *input) {
    ASTLexer l = astlexer_new(input, strlen(input));
    l.puncts = puncts;
    l.puncts_count = PUNCTS_COUNT;
    ASTLexerToken t = {0};

    ASTNode *root = astparser_expr(&l, &t);
    
    if (l.pos < l.size) {
        char *buf = malloc(128*sizeof(char));
        sprintf(buf, "Unexpected character '%c'", l.input[l.pos]);
        astparser_report_error(buf, l.pos + 1);
    }

    return root;
}
#endif // ASTPARSER
