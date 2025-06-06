#include <stdio.h>

#define ASTPARSER // By default ast.h provides only AST interface, without parsing feature
#include "ast.h"

int main() {
    const char *input = "2 * PI - e + 128 / 7";
    ASTNode *root = astparser_parse(input);
    ASTParserError err = astparser_get_error();
    if (!root) {
        printf("ERROR: %s (at %ld)\n", err.msg, err.loc);
    } else {
        printf("Eval: %g\n", ast_eval(root));
    }

    return 0;
}
