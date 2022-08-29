
/* #define PCRE_STATIC */

#include <stdio.h>
#include <string.h>
#include "pcre.h"
#include "cre.h"

/// @brief the the number of groups in a re pattern
/// @param re: re pattern
/// @param out_groups_number : from 0,1,2,3,4...
/// @return a array pointer, free if after using
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

/*************************************************************************

* (https?)://((\w+\.)+)(\w+)
* hihsid dii https://www.baidu.com, http://glwang.com
*************************************************************************/

int *pcre_match(const char *_pat, const char *s, int len, int *out_vec_number, int opt)
{
	int *vec = NULL;
	int group_n = 0;
	//int rc;
	int start_offset = 0;
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
	if (vec[0] == vec[1]) // a empty match
	{
		start_offset++; // advace a position
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
	int group_n = 0;
	//int rc;
	int start_offset = 0;
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
	opt &= ~PCRE_MULTILINE;
match:
	rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
	if (rc == PCRE_ERROR_NOMATCH)
	{
		free(vec);
		return NULL;
	}
	if (rc <= 0)
		goto e_er;
	if (vec[0] == vec[1]) // a empty match
	{
		start_offset++; // advace a position
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
	if (vec[0] == vec[1]) // a empty match
	{
		start_offset++; // advace a position
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
/// @brief find all match in a string
/// @param re: re pattern
/// @param s : string searching in
/// @param out_number : the number of matches
/// @return a vector table, vrc[n] is the nth matchs,
/// vrc[group_n][i*2] - vrc[group_n][i*2+1] is the begining-offset and ending-offset of group i.
/// Use re_free_searchall() to free the memory
int **re_searchall2(pcre *re, const char *s, int len, int *out_number, int *out_vec_number, int opt)
{
	int start_offset = 0;
	int **vecs = NULL; // to store vec
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
			goto e_er;
	match:
		int rc = pcre_exec(re, NULL, s, len, start_offset, 0, vec, group_n);
		if (rc == PCRE_ERROR_NOMATCH)
		{
			if (out_number)
				*out_number = vec_n;
			free(vec);
			return vecs;
		}
		if (rc <= 0)
			goto e_er;
		if (vec[0] == vec[1]) // a empty match
		{
			start_offset++; // advace a position
			if (start_offset >= len)
				goto e_er;
			goto match;
		}
		//to sotre vec
		if (!vecs)
		{
			vecs = (int **)malloc(sizeof(int *) * vec_cap);
			if (!vecs)
				goto e_er;
		}

		if (vec_n >= vec_cap) // need to recap this list
		{
			vec_cap *= 2;
			void *p = realloc(vecs, vec_cap * sizeof(int *));
			if (!p)
				goto e_er;
			// if (p != vecs) // move data
			// {
			// memmove(p, vecs, vec_n * sizeof(int*));
			vecs = (int **)p;
			// }
		}
		vecs[vec_n++] = vec;
		start_offset = vec[1];
	}
e_er:
	if (vec)
		free(vec); // the latest vec table
	if (!vecs)
		return NULL;
	for (int j = 0; j < vec_n; j++)
	{
		if (vecs[j])
			free((void *)(vecs[j])); // free vec table
	}
	free(vecs); // free the table list
	return NULL;
}
void re_free_searchall(int **vecs, int n)
{
	if (!vecs)
		return;
	for (int j = 0; j < n; j++)
	{
		if (vecs[j])
			free((void *)(vecs[j])); // free vec table
	}
	free(vecs); // free the table list
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
	if (vec[0] == vec[1]) // a empty match
	{
		start_offset++; // advace a position
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
			free((void *)(ss[j])); // free vec table
	}
	free(ss); // free the table list
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
char *pcre_subn(const char *pat, const char *to, const char *s, int len, int n, int opt)
{
	const char *error;
	int erroffset;
	pcre *re = pcre_compile(pat, opt, &error, &erroffset, NULL);
	if (!re)
		return NULL;
	char *res = re_subn2(re, to, s, len, n, opt);
	pcre_free(re);
	return res;
}
/// @brief substitute a string with a pattern expression, given replacement limit
/// @param re : re pattern for matching
/// @param to : re pattern to replacement
/// @param s : string searching in
/// @param len : length of <s>
/// @param n : the replacement number
/// @return if no replacement, return s exactly, otherwise return a new string, free it after using
char *re_subn2(pcre *re, const char *to, const char *s, int len, int n, int opt)
{
	int group_n = 0, group_n2 = 0;
	int *vec = NULL;
	int *vec2 = NULL;
	pcre *re2 = NULL;
	int vcs1_n = 0, vcs2_n = 0;
	int **vcs1 = re_searchall2(re, s, len, &vcs1_n, &group_n, opt);
	int **vcs2 = NULL;
	int match_limit = 0;
	if (!vcs1_n)
	{
		//no match, no replacement
		return (char *)s;
	}
	//to determine '\\' and group like: '\group_n'
	//3 groups, 0, 1, 2->\\, 3->\group_n, if any
	const char *p2 = "(\\\\\\\\|\\\\\\d{1,2})";
	int erroffset;
	const char *error;
	int len_to, remain_size, remain_length2, pi = 0, qi = 0;
	char *new_s = NULL;

	re2 = pcre_compile(p2, 0, &error, &erroffset, NULL);
	if (!re2)
		goto exit_error;
	//match <to>
	len_to = strlen(to);
	vcs2 = re_searchall2(re2, to, len_to, &vcs2_n, NULL, 0);
	//if (!vcs2)
	//{
	//	//goto exit_error;
	//	vcs2_n = 0;
	//}

	pcre_free(re2);
	re2 = NULL;
	//note that re2 is no use after this, onece we get vcs2
	remain_length2 = len_to; // the remain length in 'to' exclude from all '\\' and all '\n'
	for (int i = 0; i < vcs2_n; i++)
	{
		int *vc = vcs2[i]; // (0,1)->'\\'or'\n', (2,3)->'\\', (4,5)->'\n', (6,7,8)
		int vc0 = vc[0] + 1;
		if (to[vc0] == '\\')
		{
			vc[2] = 0;
			remain_length2 -= 2;
		}
		else // \n,
		{
			int wanted_number = 0;
			//vc[1]--;
			int l_n = vc[1] - vc0;
			if (l_n == 1)
			{
				wanted_number = to[vc0] - '0';
				remain_length2 -= 2;
			}
			else // if(l_n==2)
			{
				wanted_number = (to[vc0] - '0') * 10 + to[vc0 + 1] - '0';
				remain_length2 -= 3;
			}
			if (wanted_number <= 0 || wanted_number >= group_n)
				goto exit_error;
			//store it in vc[2]
			vc[2] = wanted_number;
		}
	}
	//now that vcs2 stores data of which group is used in replacement
	//Nx9, N is the number of groups used in every one replcaement,
	//while vcs2[2] is the exact group number used in replacement

	//parse 'to'
	//get the remian size
	match_limit = n ? (n <= vcs1_n ? n : vcs1_n) : vcs1_n;
	remain_size = len + remain_length2 * match_limit;
	//match times
	for (int i = 0; i < match_limit; i++)
	{
		int *vc = vcs1[i];
		//vc[1]-vc[0] is the match sequence which need to be replaced, while the following are groups
		remain_size -= vc[1] - vc[0];
		// the replcaements
		// 'to' e.g.: \\ \1, \2, ....\x
		for (int j = 0; j < vcs2_n; j++)
		{
			int *v2 = vcs2[j];
			if (v2[2])
			{
				//replaced to a group
				remain_size += GetGroupLen(vc, v2[2]);
			}
			else
			{
				//replaced to a '/'
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
				//replaced to a group
				memcpy(new_s + pi, s + to_group_at, g_l);
				pi += g_l;
			}
			else
			{
				//replaced to a '/'
				new_s[pi++] = '\\';
			}
			m_start = v2[1];
		}
		m_len = len_to - m_start;
		memcpy(new_s + pi, to + m_start, m_len);
		pi += m_len;
		// end of one match
		qi = vc[1];
	}
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
/// @brief substitute a string with a pattern expression
/// @param re : re pattern for matching
/// @param to : re pattern to replacement
/// @param s : string searching in
/// @param len : length of <s>
/// @return if no replacement, return s exactly, otherwise return a new string, free it after using
char *re_sub2(pcre *re, const char *to, const char *s, int len, int opt)
{
	return re_subn2(re, to, s, len, 0, opt);
}
