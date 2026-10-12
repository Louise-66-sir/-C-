#ifndef DS408_COMMON_H
#define DS408_COMMON_H
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#define CAP 256
#define MAXV 64
#define INF (INT_MAX / 4)

static void *xmalloc(size_t n)
{
    void *p = malloc(n);
    if (!p)
    {
        fputs("out of memory\n", stderr);
        exit(EXIT_FAILURE);
    }
    return p;
}

static void swap_int(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
#endif
