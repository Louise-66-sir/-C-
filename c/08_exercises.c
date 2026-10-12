/* 408 C revision: 08_exercises. Standalone teaching program. */
#include "common.h"

void reverse_range(int a[], int lo, int hi)
{ /* 闭区间。 */
    while (lo < hi)
    {
        swap_int(&a[lo++], &a[hi--]);
    }
}

bool rotate_left_array(int a[], int n, int k)
{
    if (n < 0 || k < 0)
    {
        return false;
    }
    if (n == 0)
    {
        return true;
    }
    k %= n;
    reverse_range(a, 0, k - 1);
    reverse_range(a, k, n - 1);
    reverse_range(a, 0, n - 1);
    return true;
}

int first_missing_positive(const int a[], int n)
{
    bool *seen = xmalloc(((size_t)n + 1) * sizeof *seen);
    memset(seen, 0, ((size_t)n + 1) * sizeof *seen);
    for (int i = 0; i < n; ++i)
    {
        if (a[i] > 0 && a[i] <= n)
        {
            seen[a[i]] = true;
        }
    }
    int answer = 1;
    while (answer <= n && seen[answer])
    {
        ++answer;
    }
    free(seen);
    return answer;
}

void odd_before_even(int a[], int n)
{
    int i = 0, j = n - 1;
    while (i < j)
    {
        while (i < j && a[i] % 2 != 0)
        {
            ++i;
        }
        while (i < j && a[j] % 2 == 0)
        {
            --j;
        }
        if (i < j)
        {
            swap_int(&a[i++], &a[j--]);
        }
    }
}

typedef struct ENode
{
    int data;
    struct ENode *next;
} ENode;

ENode *reverse_nodes(ENode *p)
{ /* 不带头结点的一段链，返回新首结点。 */
    ENode *pre = NULL;
    while (p)
    {
        ENode *q = p->next;
        p->next = pre;
        pre = p;
        p = q;
    }
    return pre;
}

void reorder_list(ENode *head)
{
    ENode *first = head->next;
    if (!first || !first->next)
    {
        return;
    }
    ENode *slow = first, *fast = first;
    while (fast->next && fast->next->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    ENode *right = reverse_nodes(slow->next);
    slow->next = NULL;
    ENode *left = first;
    while (right)
    {
        ENode *a = left->next, *b = right->next;
        left->next = right;
        right->next = a;
        left = a;
        right = b;
    }
}

bool palindrome_list(ENode *head)
{
    ENode *first = head->next;
    if (!first || !first->next)
    {
        return true;
    }
    ENode *slow = first, *fast = first;
    while (fast->next && fast->next->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    ENode *right = reverse_nodes(slow->next), *a = first, *b = right;
    bool ok = true;
    while (b)
    {
        if (a->data != b->data)
        {
            ok = false;
        }
        a = a->next;
        b = b->next;
    }
    slow->next = reverse_nodes(right); /* 判定后恢复，即使不回文也恢复。 */
    return ok;
}

typedef struct ETree
{
    int data;
    struct ETree *left, *right;
} ETree;

/* 叶结点data作为权；调用weighted_path_length(root,0)。 */
long long weighted_path_length(ETree *t, int depth)
{
    if (!t)
    {
        return 0;
    }
    if (!t->left && !t->right)
    {
        return (long long)t->data * depth;
    }
    return weighted_path_length(t->left, depth + 1) + weighted_path_length(t->right, depth + 1);
}

/* 调用前*k设为正整数目标次序；函数会消耗k。 */
ETree *preorder_kth(ETree *t, int *k)
{
    if (!t || *k <= 0)
    {
        return NULL;
    }
    if (--*k == 0)
    {
        return t;
    }
    ETree *p = preorder_kth(t->left, k);
    return p ? p : preorder_kth(t->right, k);
}

int path_sum_count(ETree *t, long long target)
{
    if (!t)
    {
        return 0;
    }
    if (!t->left && !t->right)
    {
        return target == t->data ? 1 : 0;
    }
    return path_sum_count(t->left, target - t->data) + path_sum_count(t->right, target - t->data);
}

/* 本练习采用0/1邻接矩阵，n<=MAXV。 */
void simple_dfs(const int a[][MAXV], int n, int u, bool seen[])
{
    seen[u] = true;
    for (int v = 0; v < n; ++v)
    {
        if (a[u][v] && !seen[v])
        {
            simple_dfs(a, n, v, seen);
        }
    }
}

/* 前提：矩阵对称、对角为0、无重边，即无向简单图。 */
bool undirected_is_tree(const int a[][MAXV], int n)
{
    if (n == 0)
    {
        return false;
    }
    int edges = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            edges += a[i][j] != 0;
        }
    }
    if (edges != n - 1)
    {
        return false;
    }
    bool seen[MAXV] = {false};
    simple_dfs(a, n, 0, seen);
    for (int i = 0; i < n; ++i)
    {
        if (!seen[i])
        {
            return false;
        }
    }
    return true;
}

typedef void (*PathVisitor)(const int path[], int length, void *context);

void simple_paths(const int a[][MAXV], int n, int u, int target, bool on_path[], int path[],
                  int depth, PathVisitor visit, void *context)
{
    on_path[u] = true;
    path[depth++] = u;
    if (u == target)
    {
        visit(path, depth, context);
    }
    else
    {
        for (int v = 0; v < n; ++v)
        {
            if (a[u][v] && !on_path[v])
            {
                simple_paths(a, n, v, target, on_path, path, depth, visit, context);
            }
        }
    }
    on_path[u] = false; /* 回溯时必须撤销，使其他路径可以再次使用该顶点。 */
}

/* 数组均非降序；结果不同值只出现一次，out容量>=min(n,m)。 */
int intersection_unique(const int a[], int n, const int b[], int m, int out[])
{
    int i = 0, j = 0, k = 0;
    while (i < n && j < m)
    {
        if (a[i] < b[j])
        {
            ++i;
        }
        else if (a[i] > b[j])
        {
            ++j;
        }
        else
        {
            if (k == 0 || out[k - 1] != a[i])
            {
                out[k++] = a[i];
            }
            ++i;
            ++j;
        }
    }
    return k;
}

void topk_down(int heap[], int n, int root)
{
    int x = heap[root], child = 2 * root + 1;
    while (child < n)
    {
        if (child + 1 < n && heap[child + 1] > heap[child])
        {
            ++child;
        }
        if (x >= heap[child])
        {
            break;
        }
        heap[root] = heap[child];
        root = child;
        child = 2 * root + 1;
    }
    heap[root] = x;
}

/* 返回最小k个数，结果保存在out中的大根堆，不保证升序；保留重复值。 */
bool smallest_k(const int a[], int n, int k, int out[])
{
    if (k < 0 || k > n)
    {
        return false;
    }
    if (k == 0)
    {
        return true;
    }
    for (int i = 0; i < k; ++i)
    {
        out[i] = a[i];
    }
    for (int i = k / 2 - 1; i >= 0; --i)
    {
        topk_down(out, k, i);
    }
    for (int i = k; i < n; ++i)
    {
        if (a[i] < out[0])
        {
            out[0] = a[i];
            topk_down(out, k, 0);
        }
    }
    return true;
}

/* Self-check entry point. */
static int exercise_cmp_test(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static ENode *exercise_list(const int a[], int n)
{
    ENode *h = xmalloc(sizeof *h), *tail = h;
    h->next = NULL;
    for (int i = 0; i < n; ++i)
    {
        ENode *p = xmalloc(sizeof *p);
        p->data = a[i];
        p->next = NULL;
        tail->next = p;
        tail = p;
    }
    return h;
}

static void exercise_free(ENode *p)
{
    while (p)
    {
        ENode *q = p->next;
        free(p);
        p = q;
    }
}

static void count_path(const int path[], int length, void *context)
{
    int *count = context;
    assert(length >= 1 && path[0] == 0 && path[length - 1] == 3);
    ++*count;
}

int main(void)
{
    srand(408);
    for (int n = 0; n <= 40; ++n)
    {
        int a[40], got[40], sorted[40];
        for (int i = 0; i < n; ++i)
        {
            a[i] = rand() % 19 - 9;
        }
        for (int k = 0; k <= 2 * n + 1; ++k)
        {
            memcpy(got, a, (size_t)n * sizeof *a);
            assert(rotate_left_array(got, n, k));
            for (int i = 0; i < n; ++i)
            {
                assert(got[i] == a[(i + k) % n]);
            }
        }
        memcpy(sorted, a, (size_t)n * sizeof *a);
        qsort(sorted, (size_t)n, sizeof *a, exercise_cmp_test);
        for (int k = 0; k <= n; ++k)
        {
            assert(smallest_k(a, n, k, got));
            qsort(got, (size_t)k, sizeof *got, exercise_cmp_test);
            for (int i = 0; i < k; ++i)
            {
                assert(got[i] == sorted[i]);
            }
        }
        memcpy(got, a, (size_t)n * sizeof *a);
        odd_before_even(got, n);
        bool even = false;
        for (int i = 0; i < n; ++i)
        {
            if (got[i] % 2 == 0)
            {
                even = true;
            }
            else
            {
                assert(!even);
            }
        }
        qsort(got, (size_t)n, sizeof *got, exercise_cmp_test);
        for (int i = 0; i < n; ++i)
        {
            assert(got[i] == sorted[i]);
        }
        ENode *h = exercise_list(a, n), *p = h->next;
        bool ref = true;
        for (int i = 0; i < n / 2; ++i)
        {
            if (a[i] != a[n - 1 - i])
            {
                ref = false;
            }
        }
        assert(palindrome_list(h) == ref);
        for (int i = 0; i < n; ++i)
        {
            assert(p && p->data == a[i]);
            p = p->next;
        }
        assert(!p);
        reorder_list(h);
        p = h->next;
        for (int i = 0; i < n; ++i)
        {
            int ix = i % 2 ? n - 1 - i / 2 : i / 2;
            assert(p && p->data == a[ix]);
            p = p->next;
        }
        assert(!p);
        exercise_free(h);
    }
    int pal[] = {1, 2, 3, 2, 1};
    ENode *h = exercise_list(pal, 5);
    assert(palindrome_list(h));
    exercise_free(h);
    int missing[] = {3, 4, -1, 1};
    assert(first_missing_positive(missing, 4) == 2);
    assert(first_missing_positive(NULL, 0) == 1);
    ETree l = {5, NULL, NULL}, r = {7, NULL, NULL}, root = {2, &l, &r};
    assert(weighted_path_length(&root, 0) == 12 && path_sum_count(&root, 7) == 1);
    int k = 2;
    assert(preorder_kth(&root, &k) == &l);
    k = 4;
    assert(!preorder_kth(&root, &k));
    int mat[MAXV][MAXV] = {{0}};
    mat[0][1] = mat[1][0] = 1;
    mat[1][2] = mat[2][1] = 1;
    assert(undirected_is_tree(mat, 3));
    mat[0][2] = mat[2][0] = 1;
    assert(!undirected_is_tree(mat, 3));
    assert(!undirected_is_tree(mat, 4));
    memset(mat, 0, sizeof mat);
    mat[0][1] = mat[0][2] = mat[1][2] = mat[2][1] = mat[1][3] = mat[2][3] = 1;
    bool on[MAXV] = {false};
    int path[MAXV], count = 0;
    simple_paths(mat, 4, 0, 3, on, path, 0, count_path, &count);
    assert(count == 4);
    for (int i = 0; i < 4; ++i)
    {
        assert(!on[i]);
    }
    int a[] = {1, 2, 2, 5}, b[] = {2, 2, 3, 5}, out[4];
    assert(intersection_unique(a, 4, b, 4, out) == 2 && out[0] == 2 && out[1] == 5);
    puts("08_exercises: PASS (arrays/lists 0..40, Top-k all k, trees, graph backtracking)");
    return 0;
}
