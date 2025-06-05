#include <stdbool.h>
#include <stdio.h>

#define ASTPARSER
#include "ast.h"
#include "sv.h"

// TODO: Think of some better interface for a calculator
void run_repl() {
    printf("*AST Calculator REPL*\nTo quit press CTRL+C or type 'q' into the input\n");
    StringView s;
    char *input;
    ASTNode *root;
    ASTParserError err;

    while (true) {
        printf("> ");
        s = sv_read_line(); sv_strip_ws(&s);
        if (sv_cmp_to_cstr(&s, "q")) {
            printf("=> Quitting!\n");
            break;
        }
        input = sv_to_cstr(&s);
        root = astparser_parse(input);
        err = astparser_get_error();
        if (!root) {
            printf("ERROR: %s\n", err.msg);
            printf("'%s'\n", input);
            printf("%*s^\n", (int)err.loc, "");
        } else {
            printf("=> %g\n", ast_eval(root));
        }
    }
}

int main() {
    run_repl();

    return 0;
}
