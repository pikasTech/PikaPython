/* 
*
*	Generally additional utility functions.
*	L flag, also known as re.LOCALE in Python is not available here.
*	Wrong results may be returned in re_sub likes funcitones when 'repl' contains '\', '\\\\1' for example. 
*
*	4/9/2022
*/
#include <stdio.h>
#include <string.h>
#include "pcre.h"
#include "cre.h"

int *_re_get_vec_table(pcre *re, int *out_groups_number)
{
	int brackets_number = 0;
	pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &brackets_number);
	brackets_number++;

	if (out_groups_number)
		*out_groups_number = brackets_number;

	brackets_number *= 3;

	int *vec = (int *)malloc(brackets_number * sizeof(int));
	return vec;
}

int *pcre_match(const char *_pat, const char *s, int len, int *out_vec_number, int opt)
{
	int *vec = NULL;
	pcre *re = re_get_match_re(_pat, opt);
	if (!re)
		return NULL;

	vec = re_match2(re, s, len, out_vec_number, opt);
	pcre_free(re);
	return vec;
}
int *re_match2(pcre *re, const char *s, int len, int *out_vec_number, int opt)
{
	int *vec = NULL;
	int group_n = 0;
	int rc;
	int start_offset = 0;
	vec = _re_get_vec_table(re, &group_n);
	if (out_vec_number)
		*out_vec_number = group_n;
	group_n *= 3;

	if (!vec)
		goto e_er;
match:
	rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
	if (rc == PCRE_ERROR_NOMATCH)
	{
		free(vec);
		return NULL;
	}
	if (rc <= 0)
		goto e_er;
	if (vec[0] == vec[1])
	{
		start_offset++;
		if (start_offset >= len)
			goto e_er;
		goto match;
	}
	return vec;
e_er:
	if (vec)
		free(vec);
	return NULL;
}

int *pcre_fullmatch(const char *_pat, const char *s, int len, int *out_vec_number, int opt)
{
	int *vec = NULL;
	opt &= ~PCRE_MULTILINE;
	pcre *re = re_get_fullmatch_re(_pat, opt);
	if (!re)
		return NULL;
	vec = re_fullmatch2(re, s, len, out_vec_number, opt);
	pcre_free(re);
	return vec;
}
int *re_fullmatch2(pcre *re, const char *s, int len, int *out_vec_number, int opt)
{
	int *vec = NULL;
	int group_n = 0;
	int rc;
	int start_offset = 0;
	vec = _re_get_vec_table(re, &group_n);
	if (out_vec_number)
		*out_vec_number = group_n;
	group_n *= 3;

	if (!vec)
		goto e_er;
	// opt &= ~PCRE_MULTILINE;
match:
	rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
	if (rc == PCRE_ERROR_NOMATCH)
	{
		free(vec);
		return NULL;
	}
	if (rc <= 0)
		goto e_er;
	if (vec[0] == vec[1])
	{
		start_offset++;
		if (start_offset >= len)
			goto e_er;
		goto match;
	}
	return vec;
e_er:
	if (vec)
		free(vec);
	return NULL;
}

pcre *re_get_match_re(const char *_pat, int opt)
{
	const char *pat = _pat;
	if (!*pat)
	{
		return NULL;
	}
	if (*pat != '^')
	{
		int pat_len = strlen(_pat);
		char *p = (char *)pcre_malloc(pat_len + 2);
		if (!p)
			return NULL;
		*p = '^';
		memcpy(p + 1, _pat, pat_len + 1);
		pat = p;
	}
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (pat != _pat)
		free((void *)pat);

	return re;
}
pcre *re_get_fullmatch_re(const char *_pat, int opt)
{
	const char *pat = _pat;
	if (!*pat)
	{
		return NULL;
	}
	int prefix = 0, suffix = 0;

	if (*pat != '^')
	{
		prefix = 1;
	}
	int pat_len = strlen(_pat);
	if (_pat[pat_len - 1] != '$')
		suffix = 1;
	else
	{
		int n = pat_len - 2;
		int i = 0;
		while (_pat[n] == '\\')
		{
			i++;
			n--;
		}
		if (i % 2)
		{
			suffix = 1;
		}
	}
	int dn = prefix + suffix;
	if (dn)
	{
		char *q = (char *)malloc(pat_len + dn + 1);
		if (!q)
			return NULL;
		pat = q;
		if (prefix)
		{
			*q = '^';
			q++;
		}
		memcpy(q, _pat, pat_len);
		q += pat_len;
		if (suffix)
		{
			*q = '$';
			q++;
		}
		*q = '\0';
	}

	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (pat != _pat)
		free((void *)pat);
	return re;
}

/* the following functions return (a) vector/table in heap, which means it need to be freed after using*/

int *pcre_search(const char *pat, const char *s, int len, int *out_vec_number, int opt)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	int *res = re_search2(re, s, len, out_vec_number, opt);
	pcre_free(re);
	return res;
}
int *re_search2(pcre *re, const char *s, int len, int *out_vec_number, int opt)
{
	int *vec = NULL;
	int group_n = 0;
	int rc;
	int start_offset = 0;
	vec = _re_get_vec_table(re, &group_n);
	if (out_vec_number)
		*out_vec_number = group_n;
	group_n *= 3;

	if (!vec)
		goto e_er;
match:
	rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
	if (rc == PCRE_ERROR_NOMATCH)
	{
		free(vec);
		return NULL;
	}
	if (rc <= 0)
		goto e_er;
	if (vec[0] == vec[1])
	{
		start_offset++;
		if (start_offset >= len)
			goto e_er;
		goto match;
	}
	return vec;
e_er:
	if (vec)
		free(vec);
	return NULL;
}

int **re_searchall(const char *pat, const char *s, int len, int *out_number, int *out_vec_number, int opt)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	int **res = re_searchall2(re, s, len, out_number, out_vec_number, opt);
	pcre_free(re);
	return res;
}
int **re_searchall2(pcre *re, const char *s, int len, int *out_number, int *out_vec_number, int opt)
{
	int start_offset = 0;
	int **vecs = NULL;
	int vec_cap = 4;
	int vec_n = 0;
	int *vec = NULL;
	int group_n = 0;

	while (1)
	{
		if (group_n)
			vec = (int *)malloc(group_n * sizeof(int));
		else
		{
			vec = _re_get_vec_table(re, &group_n);
			if (out_vec_number)
				*out_vec_number = group_n;
			group_n *= 3;
		}
		if (!vec)
		{
			goto e_er;
		}
		int rc;
	match:
		rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
		if (rc == PCRE_ERROR_NOMATCH)
		{
			if (out_number)
				*out_number = vec_n;
			free(vec);
			return vecs;
		}
		if (rc <= 0)
			goto e_er;
		if (vec[0] == vec[1])
		{
			start_offset++;
			if (start_offset >= len)
				goto e_er;
			goto match;
		}
		if (!vecs)
		{
			vecs = (int **)malloc(sizeof(int *) * vec_cap);
			if (!vecs)
				goto e_er;
		}

		if (vec_n >= vec_cap)
		{
			vec_cap *= 2;
			void *p = realloc(vecs, vec_cap * sizeof(int *));
			if (!p)
				goto e_er;
			vecs = (int **)p;
		}
		vecs[vec_n++] = vec;
		start_offset = vec[1];
	}
e_er:
	if (vec)
		free(vec);
	if (!vecs)
		return NULL;
	for (int j = 0; j < vec_n; j++)
	{
		if (vecs[j])
			free((void *)(vecs[j]));
	}
	free(vecs);
	return NULL;
}
void re_free_searchall(int **vecs, int n)
{
	if (!vecs)
		return;
	for (int j = 0; j < n; j++)
	{
		if (vecs[j])
			free((void *)(vecs[j]));
	}
	free(vecs);
}

/* the following functions return (a) string in heap, which means it need to be freed after using*/
char **_re_extract_substring(const char *s, int **vecs, int n)
{
	if (!vecs)
		return NULL;
	int c = 0;
	char **res = (char **)pcre_malloc(sizeof(char *) * n);
	if (!res)
		return NULL;
	for (int j = 0; j < n; j++)
	{
		int *v = vecs[j];
		int len = v[1] - v[0];
		char *p = (char *)pcre_malloc(len + 1);
		if (!p)
			goto e_er;
		res[c++] = p;
		memcpy(p, s + v[0], len);
		p[len] = 0;
	}
	return res;
e_er:
	if (!res)
		return NULL;
	for (int i = 0; i < c; i++)
	{
		free(res[i]);
	}
	free(res);
	return NULL;
}

char *re_find(const char *pat, const char *s, int len, int opt)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	char *res = re_find2(re, s, len, opt);
	pcre_free(re);
	return res;
}
char *re_find2(pcre *re, const char *s, int len, int opt)
{
	int *vec = NULL;
	int group_n = 0;
	int rc;
	int start_offset = 0;
	char *res_s = NULL;
	vec = _re_get_vec_table(re, &group_n);

	if (!vec)
		goto e_er;
	group_n *= 3;
match:
	rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
	if (rc == PCRE_ERROR_NOMATCH)
	{
		free(vec);
		return NULL;
	}
	if (rc <= 0)
		goto e_er;
	if (vec[0] == vec[1])
	{
		start_offset++;
		if (start_offset >= len)
			goto e_er;
		goto match;
	}
	len = vec[1] - vec[0];
	if (!len)
		goto e_er;
	res_s = (char *)malloc(len + 1);
	if (!res_s)
		goto e_er;
	memcpy(res_s, s + vec[0], len);
	res_s[len] = 0;
	if (vec)
		free(vec);
	return res_s;
e_er:
	if (vec)
		free(vec);
	return NULL;
}

char **pcre_findall(const char *pat, const char *s, int len, int *out_number, int opt)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	char **res = re_findall2(re, s, len, out_number, opt);
	pcre_free(re);
	return res;
}
char **re_findall2(pcre *re, const char *s, int len, int *out_number, int opt)
{
	int out_vec_number;
	int **vecs;
	char **res;
	vecs = re_searchall2(re, s, len, out_number, &out_vec_number, opt);
	if (!vecs)
		goto e_er;
	res = _re_extract_substring(s, vecs, *out_number);
	if (!res)
		goto e_er;
	re_free_searchall(vecs, *out_number);
	return res;
e_er:
	if (vecs)
		re_free_searchall(vecs, *out_number);
	return NULL;
}
void re_free_findall(char **ss, int n)
{
	if (!ss)
		return;
	for (int j = 0; j < n; j++)
	{
		if (ss[j])
			free((void *)(ss[j]));
	}
	free(ss);
}

char *pcre_sub(const char *pat, const char *to, const char *s, int len, int opt)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	char *res = re_sub2(re, to, s, len, opt);
	pcre_free(re);
	return res;
}
char *pcre_subn(const char *pat, const char *to, const char *s, int len, int n, int opt, int *out_repl_times)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	char *res = re_subn2(re, to, s, len, n, opt, out_repl_times);
	pcre_free(re);
	return res;
}
char *re_subn2(pcre *re, const char *to, const char *s, int len, int n, int opt, int *out_repl_times)
{
	int group_n = 0;
	pcre *re2 = NULL;
	int vcs1_n = 0, vcs2_n = 0;
	int **vcs1 = re_searchall2(re, s, len, &vcs1_n, &group_n, opt);
	int **vcs2 = NULL;
	int match_limit = 0;
	if (!vcs1_n)
	{
		return (char *)s;
	}
	const char *p2 = "(\\\\\\\\|\\\\\\d{1,2})";
	int erroffset;
	const char *error;
	int len_to, remain_size, remain_length2, pi = 0, qi = 0;
	char *new_s = NULL;

	re2 = pcre_compile(p2, 0, &error, &erroffset, NULL);
	if (!re2)
		goto exit_error;
	len_to = strlen(to);
	vcs2 = re_searchall2(re2, to, len_to, &vcs2_n, NULL, 0);
	pcre_free(re2);
	re2 = NULL;
	remain_length2 = len_to;
	for (int i = 0; i < vcs2_n; i++)
	{
		int *vc = vcs2[i];
		int vc0 = vc[0] + 1;
		if (to[vc0] == '\\')
		{
			vc[2] = 0;
			remain_length2 -= 2;
		}
		else
		{
			int wanted_number = 0;
			int l_n = vc[1] - vc0;
			if (l_n == 1)
			{
				wanted_number = to[vc0] - '0';
				remain_length2 -= 2;
			}
			else
			{
				wanted_number = (to[vc0] - '0') * 10 + to[vc0 + 1] - '0';
				remain_length2 -= 3;
			}
			if (wanted_number <= 0 || wanted_number >= group_n)
				goto exit_error;
			vc[2] = wanted_number;
		}
	}

	match_limit = n ? (n <= vcs1_n ? n : vcs1_n) : vcs1_n;
	remain_size = len + remain_length2 * match_limit;
	for (int i = 0; i < match_limit; i++)
	{
		int *vc = vcs1[i];
		remain_size -= vc[1] - vc[0];
		for (int j = 0; j < vcs2_n; j++)
		{
			int *v2 = vcs2[j];
			if (v2[2])
			{
				remain_size += GetGroupLen(vc, v2[2]);
			}
			else
			{
				remain_size++;
			}
		}
	}
	new_s = (char *)malloc(remain_size + 1);
	if (!new_s)
		goto exit_error;
	for (int i = 0; i < match_limit; i++)
	{
		int *vc = vcs1[i];
		memcpy(new_s + pi, s + qi, vc[0] - qi);
		pi += vc[0] - qi;
		int m_start = 0, m_len = 0;
		for (int j = 0; j < vcs2_n; j++)
		{
			int *v2 = vcs2[j];
			m_len = v2[0] - m_start;
			memcpy(new_s + pi, to + m_start, m_len);
			pi += m_len;

			int to_group = v2[2];
			if (to_group)
			{
				int to_group_at = vc[to_group * 2];
				int to_group_end = vc[to_group * 2 + 1];
				int g_l = to_group_end - to_group_at;
				memcpy(new_s + pi, s + to_group_at, g_l);
				pi += g_l;
			}
			else
			{
				new_s[pi++] = '\\';
			}
			m_start = v2[1];
		}
		m_len = len_to - m_start;
		memcpy(new_s + pi, to + m_start, m_len);
		pi += m_len;
		qi = vc[1];
	}
	if (out_repl_times)
		*out_repl_times = match_limit;
	if (vcs1)
		re_free_searchall(vcs1, vcs1_n);
	if (vcs2)
		re_free_searchall(vcs2, vcs2_n);
	len -= qi;
	if (len)
		memcpy(new_s + pi, s + qi, len);
	pi += len;
	new_s[pi] = '\0';
	return new_s;

exit_error:
	if (vcs1)
		re_free_searchall(vcs1, vcs1_n);
	if (vcs2)
		re_free_searchall(vcs2, vcs2_n);
	if (re2)
		pcre_free(re2);
	return NULL;
}
char *re_sub2(pcre *re, const char *to, const char *s, int len, int opt)
{
	return re_subn2(re, to, s, len, 0, opt, NULL);
}
