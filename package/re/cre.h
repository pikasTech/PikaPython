#ifndef CRE_H
#define CRE_H

#include "pcre.h"
#define GetGroupLen(vc, n) (vc[(n)*2 + 1] - vc[(n)*2])

int *_re_get_vec_table(pcre *re, int *out_groups_number);

int *pcre_match(const char *_pat, const char *s, int len, int *out_vec_number, int opt);

int *re_match2(pcre *re, const char *s, int len, int *out_vec_number, int opt);

int *pcre_fullmatch(const char *_pat, const char *s, int len, int *out_vec_number, int opt);

int *re_fullmatch2(pcre *re, const char *s, int len, int *out_vec_number, int opt);

pcre *re_get_match_re(const char *_pat, int opt);

pcre *re_get_fullmatch_re(const char *_pat, int opt);

int *pcre_search(const char *pat, const char *s, int len, int *out_vec_number, int opt);

int *re_search2(pcre *re, const char *s, int len, int *out_vec_number, int opt);

int **re_searchall(const char *pat, const char *s, int len, int *out_number, int *out_vec_number, int opt);

int **re_searchall2(pcre *re, const char *s, int len, int *out_number, int *out_vec_number, int opt);

void re_free_searchall(int **vecs, int n);

char **_re_extract_substring(const char *s, int **vecs, int n);

char *re_find(const char *pat, const char *s, int len, int opt);

char *re_find2(pcre *re, const char *s, int len, int opt);

char **pcre_findall(const char *pat, const char *s, int len, int *out_number, int opt);

char **re_findall2(pcre *re, const char *s, int len, int *out_number, int opt);

void re_free_findall(char **ss, int n);

char *pcre_sub(const char *pat, const char *to, const char *s, int len, int opt);

char *pcre_subn(const char *pat, const char *to, const char *s, int len, int n, int opt, int *out_repl_times);

char *re_subn2(pcre *re, const char *to, const char *s, int len, int n, int opt, int *out_repl_times);

char *re_sub2(pcre *re, const char *to, const char *s, int len, int opt);
#endif