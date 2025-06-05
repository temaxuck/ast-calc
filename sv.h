#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SV_GROWTH_FACTOR
#define SV_GROWTH_FACTOR 2
#endif

#ifndef SV_INITIAL_CAP
#define SV_INITIAL_CAP 64
#endif

typedef struct {
    char  *str;
    size_t len;
    size_t cap;
} StringView;

StringView sv_from_cstr(const char *cstr) {
    size_t len = strlen(cstr);
    StringView sv = {
        .str = malloc(len * sizeof(char)),
        .len = len,
        .cap = len,
    };
    memcpy(sv.str, cstr, len);
    return sv;
}

char *sv_to_cstr(StringView *sv) {
    char *cstr = malloc((sv->len + 1) * sizeof(char));
    memcpy(cstr, sv->str, sv->len);
    cstr[sv->len] = '\0';
    return cstr;
}

// Returns: false if error occured (heap overflow)
bool sv_append(StringView *sv, char c) {
    if (sv->cap == 0) {
        sv->len = 0;
        sv->cap = SV_INITIAL_CAP;
        sv->str = malloc(sv->cap * sizeof(char));
    } else if (sv->len >= sv->cap) {
        sv->cap *= SV_GROWTH_FACTOR;
        assert(sv->len <= sv->cap && "Capacity must not be less than string's length");
        sv->str = realloc(sv->str, sv->cap * sizeof(char));
    }

    if (sv->str != NULL) sv->str[sv->len++] = c;

    return sv->str != NULL;
}

StringView sv_read_line() {
    StringView s = {0};
    char c;
    while ((c = getc(stdin)) != EOF && c != '\n') sv_append(&s, c);

    return s;
}

bool sv_cmp_to_cstr(StringView *sv, const char *cstr) {
    return strcmp(sv_to_cstr(sv), cstr) == 0;
}

bool sv_starts_with_cstr(StringView *sv, const char *cstr) {
    size_t cstr_len = strlen(cstr);
    if (sv->len < cstr_len) return false;

    for (size_t i = 0; i < cstr_len; ++i) {
        if (cstr[i] != sv->str[i]) return false;
    }

    return true;
}

// Returns number of stripped space characters
int sv_strip_ws(StringView *sv) {
    if (sv->len == 0) return 0;
    size_t l = 0;
    size_t r = sv->len - 1;

    while (l < sv->len && isspace(sv->str[l])) l++;
    if (l == sv->len) {
        sv->len = 0;
        return l;
    }
    while (r > 0 && isspace(sv->str[r])) r--;
    size_t ws_count = sv->len - (r-l+1);
    sv->len = r-l+1;
    memcpy(sv->str, sv->str+l, sv->len);
    return (int)ws_count;
}
