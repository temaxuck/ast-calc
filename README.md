# AST Calculator

A basic program that parses arithmetical expressions and builds [Abstract Syntax Tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree), which can be then evaluated.

## Usage

```c
#include <stdio.h>

#define ASTPARSER // By default ast.h provides only AST interface, without parsing feature
#include "ast.h"

int main() {
    const char *input = "1.2 + 32 + e";
    ASTNode *root = astparser_parse(input);
    ASTParserError err = astparser_get_error();
    if (!root) {
        printf("ERROR: %s (at %ld)\n", err.msg, err.loc);
    } else {
        printf("Eval: %lf\n", ast_eval(root));
    }

    return 0;
}
```

### TODOs
1. Refactor `ast.h` and `astlexer.h` as STB-style libs.
