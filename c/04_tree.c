/* 408 C revision: 04_tree. Standalone teaching program. */
#include "common.h"

typedef struct BNode
{
    int data;
    struct BNode *left, *right;
} BNode;

BNode *tree_new(int x)
{
    BNode *p = xmalloc(sizeof *p);
    p->data = x;
    p->left = p->right = NULL;
    return p;
}

void tree_destroy(BNode *t)
{
    if (!t)
    {
        return;
    }
    tree_destroy(t->left);
    tree_destroy(t->right);
    free(t);
}

/* out 至少能放下全部结点；*k 入参通常为0。 */
void preorder(BNode *t, int out[], int *k)
{
    if (!t)
    {
        return;
    }
    out[(*k)++] = t->data;
    preorder(t->left, out, k);
    preorder(t->right, out, k);
}

void inorder(BNode *t, int out[], int *k)
{
    if (!t)
    {
        return;
    }
    inorder(t->left, out, k);
    out[(*k)++] = t->data;
    inorder(t->right, out, k);
}

void postorder(BNode *t, int out[], int *k)
{
    if (!t)
    {
        return;
    }
    postorder(t->left, out, k);
    postorder(t->right, out, k);
    out[(*k)++] = t->data;
}

int tree_height(BNode *t)
{
    if (!t)
    {
        return 0;
    }
    int a = tree_height(t->left), b = tree_height(t->right);
    return 1 + (a > b ? a : b);
}

int leaf_count(BNode *t)
{
    if (!t)
    {
        return 0;
    }
    if (!t->left && !t->right)
    {
        return 1;
    }
    return leaf_count(t->left) + leaf_count(t->right);
}

void mirror(BNode *t)
{
    if (!t)
    {
        return;
    }
    BNode *p = t->left;
    t->left = t->right;
    t->right = p;
    mirror(t->left);
    mirror(t->right);
}

/* 返回高度，若不平衡则返回-1，避免重复计算每个子树高度。 */
int balanced_height(BNode *t)
{
    if (!t)
    {
        return 0;
    }
    int l = balanced_height(t->left);
    if (l < 0)
    {
        return -1;
    }
    int r = balanced_height(t->right);
    if (r < 0 || abs(l - r) > 1)
    {
        return -1;
    }
    return 1 + (l > r ? l : r);
}

/* p、q 是树内两个真实结点的地址，调用者保证都存在。 */
BNode *lca(BNode *t, BNode *p, BNode *q)
{
    if (!t || t == p || t == q)
    {
        return t;
    }
    BNode *l = lca(t->left, p, q), *r = lca(t->right, p, q);
    if (l && r)
    {
        return t;
    }
    return l ? l : r;
}

int inorder_iter(BNode *t, int out[])
{
    BNode *st[CAP], *p = t;
    int top = 0, k = 0;
    while (p || top)
    {
        while (p)
        {
            st[top++] = p;
            p = p->left;
        }
        p = st[--top];
        out[k++] = p->data;
        p = p->right;
    }
    return k;
}

int preorder_iter(BNode *t, int out[])
{
    BNode *st[CAP];
    int top = 0, k = 0;
    if (t)
    {
        st[top++] = t;
    }
    while (top)
    {
        BNode *p = st[--top];
        out[k++] = p->data;
        if (p->right)
        {
            st[top++] = p->right; /* 右先入，左先出。 */
        }
        if (p->left)
        {
            st[top++] = p->left;
        }
    }
    return k;
}

int postorder_iter(BNode *t, int out[])
{
    BNode *st[CAP], *p = t, *last = NULL;
    int top = 0, k = 0;
    while (p || top)
    {
        if (p)
        {
            st[top++] = p;
            p = p->left;
        }
        else
        {
            BNode *q = st[top - 1];
            if (q->right && last != q->right)
            {
                p = q->right;
            }
            else
            {
                out[k++] = q->data;
                last = q;
                --top;
            }
        }
    }
    return k;
}

int levelorder(BNode *t, int out[])
{
    BNode *q[CAP];
    int f = 0, r = 0, k = 0;
    if (t)
    {
        q[r++] = t;
    }
    while (f < r)
    {
        BNode *p = q[f++];
        out[k++] = p->data;
        if (p->left)
        {
            q[r++] = p->left;
        }
        if (p->right)
        {
            q[r++] = p->right;
        }
    }
    return k;
}

int tree_width(BNode *t)
{
    BNode *q[CAP];
    int f = 0, r = 0, best = 0;
    if (t)
    {
        q[r++] = t;
    }
    while (f < r)
    {
        int end = r, width = r - f;
        if (width > best)
        {
            best = width;
        }
        while (f < end)
        {
            BNode *p = q[f++];
            if (p->left)
            {
                q[r++] = p->left;
            }
            if (p->right)
            {
                q[r++] = p->right;
            }
        }
    }
    return best;
}

bool is_complete(BNode *t)
{
    BNode *q[2 * CAP + 1];
    int f = 0, r = 0;
    bool gap = false;
    q[r++] = t;
    while (f < r)
    {
        BNode *p = q[f++];
        if (!p)
        {
            gap = true;
        }
        else
        {
            if (gap)
            {
                return false;
            }
            q[r++] = p->left;
            q[r++] = p->right;
        }
    }
    return true;
}

/* 前提：元素互异，pre/in合法且包含相同元素；区间长度n。 */
BNode *build_pre_in(const int pre[], const int in[], int n)
{
    if (n == 0)
    {
        return NULL;
    }
    int k = 0;
    while (k < n && in[k] != pre[0])
    {
        ++k;
    }
    assert(k < n);
    BNode *t = tree_new(pre[0]);
    t->left = build_pre_in(pre + 1, in, k);
    t->right = build_pre_in(pre + 1 + k, in + k + 1, n - k - 1);
    return t;
}

typedef struct TNode
{
    int data;
    struct TNode *left, *right;
    bool ltag, rtag; /* false=孩子，true=线索。 */
} TNode;

TNode *thread_copy(BNode *t)
{
    if (!t)
    {
        return NULL;
    }
    TNode *p = xmalloc(sizeof *p);
    p->data = t->data;
    p->ltag = p->rtag = false;
    p->left = thread_copy(t->left);
    p->right = thread_copy(t->right);
    return p;
}

void thread_visit(TNode *t, TNode **pre)
{
    if (!t)
    {
        return;
    }
    if (!t->ltag)
    {
        thread_visit(t->left, pre);
    }
    if (!t->left)
    {
        t->ltag = true;
        t->left = *pre;
    }
    if (*pre && !(*pre)->right)
    {
        (*pre)->rtag = true;
        (*pre)->right = t;
    }
    *pre = t;
    if (!t->rtag)
    {
        thread_visit(t->right, pre);
    }
}

/* 输入必须是尚未线索化的树；无头结点版本。 */
void make_inorder_threads(TNode *t)
{
    TNode *pre = NULL;
    thread_visit(t, &pre);
    if (pre && !pre->right)
    {
        pre->rtag = true;
    }
}

TNode *thread_first(TNode *t)
{
    while (t && !t->ltag)
    {
        t = t->left;
    }
    return t;
}

TNode *thread_next(TNode *p)
{
    if (!p)
    {
        return NULL;
    }
    return p->rtag ? p->right : thread_first(p->right);
}

TNode *thread_prev(TNode *p)
{
    if (!p)
    {
        return NULL;
    }
    if (p->ltag)
    {
        return p->left;
    }
    p = p->left;
    while (p && !p->rtag)
    {
        p = p->right;
    }
    return p;
}

void thread_destroy(TNode *t)
{
    if (!t)
    {
        return;
    }
    if (!t->ltag)
    {
        thread_destroy(t->left);
    }
    if (!t->rtag)
    {
        thread_destroy(t->right);
    }
    free(t);
}

typedef struct GNode
{
    int data;
    struct GNode *first_child, *next_sibling;
} GNode;

/* 输入森林的第一棵树根；每个兄弟都在相同层。 */
int forest_height(GNode *t)
{
    if (!t)
    {
        return 0;
    }
    int own = 1 + forest_height(t->first_child);
    int rest = forest_height(t->next_sibling);
    return own > rest ? own : rest;
}

/* 用数组表示树：前n个为叶，后面为内部结点。n在1..CAP。 */
typedef struct
{
    int weight, parent, left, right;
} HNode;

int huffman_build(const int w[], int n, HNode h[])
{
    assert(n >= 1 && n <= CAP);
    for (int i = 0; i < 2 * n - 1; ++i)
    {
        h[i] = (HNode){i < n ? w[i] : 0, -1, -1, -1};
        if (i < n)
        {
            assert(w[i] > 0);
        }
    }
    int wpl = 0;
    for (int i = n; i < 2 * n - 1; ++i)
    {
        int a = -1, b = -1;
        for (int j = 0; j < i; ++j)
        {
            if (h[j].parent == -1)
            {
                if (a == -1 || h[j].weight < h[a].weight)
                {
                    b = a;
                    a = j;
                }
                else if (b == -1 || h[j].weight < h[b].weight)
                {
                    b = j;
                }
            }
        }
        h[a].parent = h[b].parent = i;
        h[i].left = a;
        h[i].right = b;
        h[i].weight = h[a].weight + h[b].weight;
        wpl += h[i].weight;
    }
    return wpl;
}

/* 从叶向根产生反向编码后逆置，out容量至少CAP+1。单符号约定为"0"。 */
void huffman_code(const HNode h[], int leaf, char out[])
{
    int len = 0, child = leaf, parent = h[child].parent;
    if (parent == -1)
    {
        strcpy(out, "0");
        return;
    }
    while (parent != -1)
    {
        out[len++] = h[parent].left == child ? '0' : '1';
        child = parent;
        parent = h[child].parent;
    }
    for (int i = 0; i < len / 2; ++i)
    {
        char c = out[i];
        out[i] = out[len - 1 - i];
        out[len - 1 - i] = c;
    }
    out[len] = '\0';
}

/* Self-check entry point. */
static void compare_traversals(BNode *t)
{
    int a[CAP], b[CAP], n = 0;
    preorder(t, a, &n);
    assert(preorder_iter(t, b) == n);
    for (int i = 0; i < n; ++i)
    {
        assert(a[i] == b[i]);
    }
    n = 0;
    inorder(t, a, &n);
    assert(inorder_iter(t, b) == n);
    for (int i = 0; i < n; ++i)
    {
        assert(a[i] == b[i]);
    }
    TNode *thread = thread_copy(t);
    make_inorder_threads(thread);
    TNode *p = thread_first(thread), *previous = NULL;
    for (int i = 0; i < n; ++i)
    {
        assert(p && p->data == a[i] && thread_prev(p) == previous);
        previous = p;
        p = thread_next(p);
    }
    assert(!p);
    thread_destroy(thread);
    n = 0;
    postorder(t, a, &n);
    assert(postorder_iter(t, b) == n);
    for (int i = 0; i < n; ++i)
    {
        assert(a[i] == b[i]);
    }
}

static BNode *random_tree(int n, int *key)
{
    if (n == 0)
    {
        return NULL;
    }
    int left = rand() % n;
    BNode *t = tree_new((*key)++);
    t->left = random_tree(left, key);
    t->right = random_tree(n - 1 - left, key);
    return t;
}

int main(void)
{
    compare_traversals(NULL);
    assert(is_complete(NULL) && tree_height(NULL) == 0);
    int pre[] = {1, 2, 4, 5, 3, 6}, in[] = {4, 2, 5, 1, 3, 6};
    BNode *t = build_pre_in(pre, in, 6);
    compare_traversals(t);
    assert(tree_height(t) == 3 && leaf_count(t) == 3 && tree_width(t) == 3);
    assert(!is_complete(t) && balanced_height(t) == 3);
    assert(lca(t, t->left->left, t->left->right) == t->left);
    int out[CAP];
    assert(levelorder(t, out) == 6 && out[0] == 1 && out[5] == 6);
    mirror(t);
    mirror(t);
    compare_traversals(t);
    tree_destroy(t);
    srand(408);
    for (int trial = 0; trial < 300; ++trial)
    {
        int key = 0;
        t = random_tree(trial % 70, &key);
        compare_traversals(t);
        tree_destroy(t);
    }
    for (int n = 1; n < 30; ++n)
    {
        BNode *nodes[30];
        for (int i = 0; i < n; ++i)
        {
            nodes[i] = tree_new(i);
        }
        for (int i = 1; i < n; ++i)
        {
            if (i % 2)
            {
                nodes[(i - 1) / 2]->left = nodes[i];
            }
            else
            {
                nodes[(i - 1) / 2]->right = nodes[i];
            }
        }
        assert(is_complete(nodes[0]));
        tree_destroy(nodes[0]);
    }
    t = tree_new(1);
    t->left = tree_new(2);
    t->left->left = tree_new(3);
    assert(balanced_height(t) == -1);
    tree_destroy(t);
    int w[] = {2, 3, 7, 9};
    HNode h[2 * CAP];
    char codes[4][CAP + 1];
    assert(huffman_build(w, 4, h) == 38);
    int wpl = 0;
    for (int i = 0; i < 4; ++i)
    {
        huffman_code(h, i, codes[i]);
        wpl += w[i] * (int)strlen(codes[i]);
    }
    assert(wpl == 38);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i != j)
            {
                assert(strncmp(codes[i], codes[j], strlen(codes[i])) != 0);
            }
        }
    }
    assert(huffman_build(w, 1, h) == 0);
    huffman_code(h, 0, codes[0]);
    assert(!strcmp(codes[0], "0"));
    GNode c = {3, NULL, NULL}, b = {2, NULL, &c}, a = {1, &b, NULL};
    assert(forest_height(&a) == 2);
    puts("04_tree: PASS (300 random trees, recursive/iterative/threaded equivalence, Huffman)");
    return 0;
}
