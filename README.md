# AST Calculator

A basic program that parses arithmetical expressions and builds [Abstract Syntax Tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree), which can be then evaluated. The project was created for educational and recreational purposes, so please don't expect much from it.

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
        printf("Eval: %g\n", ast_eval(root));
    }

    return 0;
}
```
This example produces this output:
```
ERROR: Unexpected character 'e' (at 12)
```

## REPL
You can use REPL to test the AST Calculator:
1. Build and run it: `make repl`;
2. Use REPL:
```
*AST Calculator REPL*
To quit press CTRL+C or type 'q' into the input
> cool
ERROR: Unexpected character 'c'
'cool'
 ^
> 321 + 213
=> 534
> 123 + 2 * 3 * 5 / 3
=> 133
```

### TODOs
1. Refactor `ast.h` and `astlexer.h` as STB-style libs.
2. Add more operators (~~raise_to_power~~, log, ln, cos, sin, tan); 
3. ~~Add mathematical constants (e, pi, etc.)~~
4. ~~Add REPL~~
