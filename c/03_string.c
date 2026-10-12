/* 408 C revision: 03_string. Standalone teaching program. */
#include "common.h"

int bf_find(const char *s, const char *p)
{
    int n = (int)strlen(s), m = (int)strlen(p);
    if (m == 0)
    {
        return 0;
    }
    for (int i = 0; i + m <= n; ++i)
    {
        int j = 0;
        while (j < m && s[i + j] == p[j])
        {
            ++j;
        }
        if (j == m)
        {
            return i;
        }
    }
    return -1;
}

void build_pi(const char *p, int pi[])
{
    int m = (int)strlen(p);
    if (m == 0)
    {
        return;
    }
    pi[0] = 0;
    for (int i = 1, j = 0; i < m; ++i)
    {
        while (j > 0 && p[i] != p[j])
        {
            j = pi[j - 1];
        }
        if (p[i] == p[j])
        {
            ++j;
        }
        pi[i] = j;
    }
}

int kmp_find(const char *s, const char *p)
{
    int n = (int)strlen(s), m = (int)strlen(p);
    if (m == 0)
    {
        return 0;
    }
    int *pi = xmalloc((size_t)m * sizeof *pi);
    build_pi(p, pi);
    for (int i = 0, j = 0; i < n; ++i)
    {
        while (j > 0 && s[i] != p[j])
        {
            j = pi[j - 1];
        }
        if (s[i] == p[j])
        {
            ++j;
        }
        if (j == m)
        {
            free(pi);
            return i - m + 1;
        }
    }
    free(pi);
    return -1;
}

/* 返回全部出现次数，包括重叠匹配；空模式按 n+1 个边界计数。 */
int kmp_count(const char *s, const char *p)
{
    int n = (int)strlen(s), m = (int)strlen(p), count = 0;
    if (m == 0)
    {
        return n + 1;
    }
    int *pi = xmalloc((size_t)m * sizeof *pi);
    build_pi(p, pi);
    for (int i = 0, j = 0; i < n; ++i)
    {
        while (j > 0 && s[i] != p[j])
        {
            j = pi[j - 1];
        }
        if (s[i] == p[j])
        {
            ++j;
        }
        if (j == m)
        {
            ++count;
            j = pi[j - 1];
        }
    }
    free(pi);
    return count;
}

void build_nextval(const char *p, int nv[])
{
    int m = (int)strlen(p);
    if (m == 0)
    {
        return;
    }
    int i = 0, j = -1;
    nv[0] = -1;
    while (i + 1 < m)
    {
        if (j == -1 || p[i] == p[j])
        {
            ++i;
            ++j;
            nv[i] = p[i] == p[j] ? nv[j] : j;
        }
        else
        {
            j = nv[j];
        }
    }
}

int kmp_nextval_find(const char *s, const char *p)
{
    int n = (int)strlen(s), m = (int)strlen(p);
    if (m == 0)
    {
        return 0;
    }
    int *nv = xmalloc((size_t)m * sizeof *nv);
    build_nextval(p, nv);
    int i = 0, j = 0;
    while (i < n && j < m)
    {
        if (j == -1 || s[i] == p[j])
        {
            ++i;
            ++j;
        }
        else
        {
            j = nv[j];
        }
    }
    free(nv);
    return j == m ? i - m : -1;
}

/* Self-check entry point. */
static void binary_word(char out[], int n, int mask)
{
    for (int i = 0; i < n; ++i)
    {
        out[i] = (mask & (1 << i)) ? 'b' : 'a';
    }
    out[n] = '\0';
}

int main(void)
{
    int pi[7], expected[] = {0, 0, 1, 2, 3, 0, 1};
    build_pi("ababaca", pi);
    for (int i = 0; i < 7; ++i)
    {
        assert(pi[i] == expected[i]);
    }
    assert(kmp_count("aaaaa", "aaa") == 3 && kmp_count("abc", "") == 4);
    int cases = 0;
    for (int n = 0; n <= 7; ++n)
    {
        for (int a = 0; a < (1 << n); ++a)
        {
            for (int m = 0; m <= 5; ++m)
            {
                for (int b = 0; b < (1 << m); ++b)
                {
                    char s[8], p[6];
                    binary_word(s, n, a);
                    binary_word(p, m, b);
                    int ref = bf_find(s, p);
                    assert(kmp_find(s, p) == ref && kmp_nextval_find(s, p) == ref);
                    int count = 0;
                    for (int i = 0; i + m <= n; ++i)
                    {
                        if (strncmp(s + i, p, (size_t)m) == 0)
                        {
                            ++count;
                        }
                    }
                    assert(kmp_count(s, p) == count);
                    ++cases;
                }
            }
        }
    }
    printf("03_string: PASS (%d exhaustive binary text/pattern pairs)\n", cases);
    return 0;
}
