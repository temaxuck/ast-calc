/* astlexer.h - A lexer intended for parsing simple mathematical expressions.
 *
 * It was inspired by alexer.h (https://github.com/tsoding/alexer).
 */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { ASTLEXER_NUMBER, ASTLEXER_PUNCT, ASTLEXER_EOI } ASTLexerKind;

typedef struct {
    ASTLexerKind t;
    const char *start;
    const char *end;
    size_t loc; // Position of the first token character in input string
    double numval;
} ASTLexerToken;

typedef struct {
    const char *input;
    size_t size;
    size_t pos;

    const char **puncts;
    size_t puncts_count;
} ASTLexer;

ASTLexer astlexer_new(const char *input, size_t size) {
    return (ASTLexer) {
        .input = input,
        .size = size,
    };
}

bool astlexer_next_char(ASTLexer *l) {
    if (l->pos >= l->size) return false;
    l->pos++; return true;
}

void astlexer_next_n_chars(ASTLexer *l, size_t n) {
    while (n-- > 0 && astlexer_next_char(l));
}

void astlexer_skip_ws(ASTLexer *l) {
    while (isspace(l->input[l->pos]) && astlexer_next_char(l));
}

bool astlexer_starts_with_cstr(ASTLexer *l, const char *prefix) {
    for (size_t i = 0; l->pos + i < l->size && prefix[i] != '\0'; i++) {
        if (l->input[l->pos + i] != prefix[i]) return false;
    }

    return true;
}

bool astlexer_get_token(ASTLexer *l, ASTLexerToken *t) {
    astlexer_skip_ws(l);
    
    memset(t, 0, sizeof(*t));
    t->loc = l->pos;
    t->start = &l->input[l->pos];
    t->end = &l->input[l->pos];

    if (l->pos >= l->size) {
        t->t = ASTLEXER_EOI;
        return false;
    }

    if (isdigit(l->input[l->pos])) {
        t->t = ASTLEXER_NUMBER;
        char *end;
        t->numval = strtod(&l->input[l->pos], &end);
        t->end = end;
        astlexer_next_n_chars(l, t->end - t->start);

        return true;
    }

    for (size_t i = 0; i < l->puncts_count; i++) {
        if (astlexer_starts_with_cstr(l, l->puncts[i])) {
            t->t = ASTLEXER_PUNCT;
            size_t n = strlen(l->puncts[i]);
            t->end += n;
            astlexer_next_n_chars(l, n);

            return true;
        }
    }

    return false;
}

bool astlexer_peek(ASTLexer *l, ASTLexerToken *t) {
    size_t prev_pos = l->pos;
    bool successful = astlexer_get_token(l, t);
    l->pos = prev_pos;

    return successful;
}


char *astlexer_token_to_cstr(const ASTLexerToken *t) {
    size_t len = t->end - t->start;
    char *buf = malloc(len + 1);
    if (!buf) return NULL;

    memcpy(buf, t->start, len);
    buf[len] = '\0';

    return buf;
}

bool astlexer_token_cmp_cstr(const ASTLexerToken *t, const char *cstr) {
    return strcmp(astlexer_token_to_cstr(t), cstr) == 0;
}
