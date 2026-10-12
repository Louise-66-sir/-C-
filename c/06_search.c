/* 408 C revision: 06_search. Standalone teaching program. */
#include "common.h"

int sequential_find(const int a[], int n, int key)
{
    for (int i = 0; i < n; ++i)
    {
        if (a[i] == key)
        {
            return i;
        }
    }
    return -1;
}

/* 闭区间[low,high]；存在重复值时返回其中任意一个。 */
int binary_find(const int a[], int n, int key)
{
    int low = 0, high = n - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        if (a[mid] == key)
        {
            return mid;
        }
        if (a[mid] < key)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1;
}

/* 半开区间[low,high)，返回第一个>=key的位置，可能为n。 */
int lower_bound_c(const int a[], int n, int key)
{
    int low = 0, high = n;
    while (low < high)
    {
        int mid = low + (high - low) / 2;
        if (a[mid] < key)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }
    return low;
}

/* 返回第一个>key的位置。 */
int upper_bound_c(const int a[], int n, int key)
{
    int low = 0, high = n;
    while (low < high)
    {
        int mid = low + (high - low) / 2;
        if (a[mid] <= key)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }
    return low;
}

/* 块间有序：前一块所有键<后一块所有键；max为块内最大值。
   start/end是各块在数组中的闭区间，块内可无序。 */
typedef struct
{
    int max, start, end;
} Block;

int block_find(const int a[], const Block b[], int blocks, int key)
{
    int low = 0, high = blocks;
    while (low < high)
    {
        int mid = low + (high - low) / 2;
        if (b[mid].max < key)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }
    if (low == blocks)
    {
        return -1;
    }
    for (int i = b[low].start; i <= b[low].end; ++i)
    {
        if (a[i] == key)
        {
            return i;
        }
    }
    return -1;
}

typedef struct BST
{
    int key;
    struct BST *left, *right;
} BST;

BST *bst_new(int key)
{
    BST *t = xmalloc(sizeof *t);
    t->key = key;
    t->left = t->right = NULL;
    return t;
}

BST *bst_find(BST *t, int key)
{
    while (t && t->key != key)
    {
        t = key < t->key ? t->left : t->right;
    }
    return t;
}

/* 本例集合不存重复关键字；必须用root=bst_insert(root,key)接新根。 */
BST *bst_insert(BST *t, int key)
{
    if (!t)
    {
        return bst_new(key);
    }
    if (key < t->key)
    {
        t->left = bst_insert(t->left, key);
    }
    else if (key > t->key)
    {
        t->right = bst_insert(t->right, key);
    }
    return t;
}

BST *bst_erase(BST *t, int key)
{
    if (!t)
    {
        return NULL;
    }
    if (key < t->key)
    {
        t->left = bst_erase(t->left, key);
    }
    else if (key > t->key)
    {
        t->right = bst_erase(t->right, key);
    }
    else
    {
        if (!t->left || !t->right)
        {
            BST *child = t->left ? t->left : t->right;
            free(t);
            return child;
        }
        BST *s = t->right;
        while (s->left)
        {
            s = s->left;
        }
        t->key = s->key;
        t->right = bst_erase(t->right, s->key);
    }
    return t;
}

void bst_destroy(BST *t)
{
    if (!t)
    {
        return;
    }
    bst_destroy(t->left);
    bst_destroy(t->right);
    free(t);
}

typedef struct AVL
{
    int key, h;
    struct AVL *left, *right;
} AVL;

int avl_h(AVL *t)
{
    return t ? t->h : 0;
}

void avl_update(AVL *t)
{
    int l = avl_h(t->left), r = avl_h(t->right);
    t->h = 1 + (l > r ? l : r);
}

AVL *rotate_right(AVL *y)
{
    AVL *x = y->left, *middle = x->right;
    x->right = y;
    y->left = middle;
    avl_update(y);
    avl_update(x);
    return x; /* 先更新旧根。 */
}

AVL *rotate_left(AVL *x)
{
    AVL *y = x->right, *middle = y->left;
    y->left = x;
    x->right = middle;
    avl_update(x);
    avl_update(y);
    return y;
}

AVL *avl_insert(AVL *t, int key)
{
    if (!t)
    {
        AVL *p = xmalloc(sizeof *p);
        *p = (AVL){key, 1, NULL, NULL};
        return p;
    }
    if (key < t->key)
    {
        t->left = avl_insert(t->left, key);
    }
    else if (key > t->key)
    {
        t->right = avl_insert(t->right, key);
    }
    else
    {
        return t;
    }
    avl_update(t);
    int balance = avl_h(t->left) - avl_h(t->right);
    if (balance > 1)
    {
        if (key > t->left->key)
        {
            t->left = rotate_left(t->left); /* LR。 */
        }
        return rotate_right(t); /* LL或LR的第二步。 */
    }
    if (balance < -1)
    {
        if (key < t->right->key)
        {
            t->right = rotate_right(t->right); /* RL。 */
        }
        return rotate_left(t); /* RR或RL的第二步。 */
    }
    return t;
}

void avl_destroy(AVL *t)
{
    if (!t)
    {
        return;
    }
    avl_destroy(t->left);
    avl_destroy(t->right);
    free(t);
}

AVL *avl_erase(AVL *t, int key)
{
    if (!t)
    {
        return NULL;
    }
    if (key < t->key)
    {
        t->left = avl_erase(t->left, key);
    }
    else if (key > t->key)
    {
        t->right = avl_erase(t->right, key);
    }
    else
    {
        if (!t->left || !t->right)
        {
            AVL *child = t->left ? t->left : t->right;
            free(t);
            return child;
        }
        AVL *s = t->right;
        while (s->left)
        {
            s = s->left;
        }
        t->key = s->key;
        t->right = avl_erase(t->right, s->key);
    }
    avl_update(t);
    int balance = avl_h(t->left) - avl_h(t->right);
    if (balance > 1)
    {
        if (avl_h(t->left->left) < avl_h(t->left->right))
        {
            t->left = rotate_left(t->left);
        }
        return rotate_right(t);
    }
    if (balance < -1)
    {
        if (avl_h(t->right->right) < avl_h(t->right->left))
        {
            t->right = rotate_right(t->right);
        }
        return rotate_left(t);
    }
    return t;
}

#define BT_ORDER 5

typedef struct BTreeNode
{
    int n, key[BT_ORDER - 1];
    bool leaf;
    struct BTreeNode *child[BT_ORDER];
} BTreeNode;

/* 仅演示合法B树的查找；slot返回在命中结点内的位置。 */
BTreeNode *btree_find(BTreeNode *t, int key, int *slot)
{
    while (t)
    {
        int i = 0;
        while (i < t->n && key > t->key[i])
        {
            ++i;
        }
        if (i < t->n && key == t->key[i])
        {
            *slot = i;
            return t;
        }
        if (t->leaf)
        {
            return NULL;
        }
        t = t->child[i];
    }
    return NULL;
}

typedef struct BPlus
{
    int n, key[BT_ORDER];
    bool leaf;
    struct BPlus *child[BT_ORDER], *next;
} BPlus;

/* 内部结点key[i]为child[i]子树最大键；命中也必须继续到叶。 */
BPlus *bplus_find(BPlus *t, int key, int *slot)
{
    while (t && !t->leaf)
    {
        int i = 0;
        while (i < t->n && t->key[i] < key)
        {
            ++i;
        }
        if (i == t->n)
        {
            return NULL;
        }
        t = t->child[i];
    }
    if (!t)
    {
        return NULL;
    }
    int i = lower_bound_c(t->key, t->n, key);
    if (i < t->n && t->key[i] == key)
    {
        *slot = i;
        return t;
    }
    return NULL;
}

#define HM 17

typedef enum
{
    EMPTY,
    USED,
    DELETED
} State;

typedef struct
{
    int key[HM];
    State state[HM];
} Hash;

int hash_home(int key)
{
    return (key % HM + HM) % HM;
}

void hash_init(Hash *h)
{
    for (int i = 0; i < HM; ++i)
    {
        h->state[i] = EMPTY;
    }
}

int hash_find(const Hash *h, int key)
{
    int start = hash_home(key);
    for (int k = 0; k < HM; ++k)
    {
        int i = (start + k) % HM;
        if (h->state[i] == EMPTY)
        {
            return -1;
        }
        if (h->state[i] == USED && h->key[i] == key)
        {
            return i;
        }
    }
    return -1;
}

/* 集合语义，已有键视为成功；插入必须探测到EMPTY或整表结束再判重复。 */
bool hash_insert(Hash *h, int key)
{
    int start = hash_home(key), first_deleted = -1;
    for (int k = 0; k < HM; ++k)
    {
        int i = (start + k) % HM;
        if (h->state[i] == USED && h->key[i] == key)
        {
            return true;
        }
        if (h->state[i] == DELETED && first_deleted == -1)
        {
            first_deleted = i;
        }
        if (h->state[i] == EMPTY)
        {
            if (first_deleted != -1)
            {
                i = first_deleted;
            }
            h->key[i] = key;
            h->state[i] = USED;
            return true;
        }
    }
    if (first_deleted != -1)
    {
        h->key[first_deleted] = key;
        h->state[first_deleted] = USED;
        return true;
    }
    return false;
}

bool hash_erase(Hash *h, int key)
{
    int i = hash_find(h, key);
    if (i == -1)
    {
        return false;
    }
    h->state[i] = DELETED;
    return true;
}

typedef struct HLink
{
    int key;
    struct HLink *next;
} HLink;

typedef struct
{
    HLink *bucket[HM];
} ChainHash;

void chain_init(ChainHash *h)
{
    memset(h, 0, sizeof *h);
}

bool chain_has(const ChainHash *h, int key)
{
    for (HLink *p = h->bucket[hash_home(key)]; p; p = p->next)
    {
        if (p->key == key)
        {
            return true;
        }
    }
    return false;
}

void chain_insert(ChainHash *h, int key)
{
    if (chain_has(h, key))
    {
        return;
    }
    int k = hash_home(key);
    HLink *p = xmalloc(sizeof *p);
    p->key = key;
    p->next = h->bucket[k];
    h->bucket[k] = p;
}

bool chain_erase(ChainHash *h, int key)
{
    HLink **link = &h->bucket[hash_home(key)];
    while (*link && (*link)->key != key)
    {
        link = &(*link)->next;
    }
    if (!*link)
    {
        return false;
    }
    HLink *p = *link;
    *link = p->next;
    free(p);
    return true;
}

void chain_destroy(ChainHash *h)
{
    for (int i = 0; i < HM; ++i)
    {
        HLink *p = h->bucket[i];
        while (p)
        {
            HLink *q = p->next;
            free(p);
            p = q;
        }
        h->bucket[i] = NULL;
    }
}

/* Self-check entry point. */
static int int_cmp_test(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static int verify_avl(AVL *t, int low, int high)
{
    if (!t)
    {
        return 0;
    }
    assert(t->key > low && t->key < high);
    int l = verify_avl(t->left, low, t->key), r = verify_avl(t->right, t->key, high);
    assert(abs(l - r) <= 1 && t->h == 1 + (l > r ? l : r));
    return t->h;
}

int main(void)
{
    srand(408);
    for (int trial = 0; trial < 300; ++trial)
    {
        int a[80], n = trial % 80;
        for (int i = 0; i < n; ++i)
        {
            a[i] = rand() % 31 - 15;
        }
        qsort(a, (size_t)n, sizeof *a, int_cmp_test);
        for (int key = -17; key <= 17; ++key)
        {
            int l = 0, r = 0;
            while (l < n && a[l] < key)
            {
                ++l;
            }
            while (r < n && a[r] <= key)
            {
                ++r;
            }
            assert(lower_bound_c(a, n, key) == l && upper_bound_c(a, n, key) == r);
            int found = binary_find(a, n, key);
            assert((found == -1) == (l == r));
            if (found != -1)
            {
                assert(a[found] == key);
            }
            assert((sequential_find(a, n, key) == -1) == (l == r));
        }
    }
    int a[] = {3, 1, 2, 8, 7, 6};
    Block b[] = {{3, 0, 2}, {8, 3, 5}};
    assert(block_find(a, b, 2, 7) == 4 && block_find(a, b, 2, 4) == -1);
    bool present[101] = {false};
    BST *root = NULL;
    AVL *avl = NULL;
    for (int i = 0; i < 800; ++i)
    {
        int key = rand() % 101;
        root = bst_insert(root, key);
        avl = avl_insert(avl, key);
        present[key] = true;
        verify_avl(avl, -1, 101);
    }
    for (int key = 0; key < 101; ++key)
    {
        assert((bst_find(root, key) != NULL) == present[key]);
    }
    for (int key = 0; key < 101; ++key)
    {
        root = bst_erase(root, key);
        assert(!bst_find(root, key));
    }
    assert(!root);
    for (int key = 0; key < 101; ++key)
    {
        avl = avl_erase(avl, key);
        verify_avl(avl, -1, 101);
    }
    assert(!avl);
    avl_destroy(avl);
    bst_destroy(root);
    bool avl_set[101] = {false};
    for (int step = 0; step < 1200; ++step)
    {
        int key = rand() % 101;
        if (rand() % 2)
        {
            avl = avl_insert(avl, key);
            avl_set[key] = true;
        }
        else
        {
            avl = avl_erase(avl, key);
            avl_set[key] = false;
        }
        verify_avl(avl, -1, 101);
        for (int i = 0; i < 101; ++i)
        {
            AVL *p = avl;
            while (p && p->key != i)
            {
                p = i < p->key ? p->left : p->right;
            }
            assert((p != NULL) == avl_set[i]);
        }
    }
    avl_destroy(avl);
    Hash h;
    hash_init(&h);
    bool set[101] = {false};
    int used = 0;
    for (int step = 0; step < 5000; ++step)
    {
        int ix = rand() % 101, key = ix - 50;
        if (rand() % 3)
        {
            bool ok = hash_insert(&h, key);
            assert(ok == (set[ix] || used < HM));
            if (ok && !set[ix])
            {
                set[ix] = true;
                ++used;
            }
        }
        else
        {
            assert(hash_erase(&h, key) == set[ix]);
            if (set[ix])
            {
                set[ix] = false;
                --used;
            }
        }
        for (int i = 0; i < 101; ++i)
        {
            assert((hash_find(&h, i - 50) != -1) == set[i]);
        }
    }
    ChainHash ch;
    chain_init(&ch);
    for (int i = -50; i <= 50; ++i)
    {
        chain_insert(&ch, i);
        chain_insert(&ch, i);
    }
    for (int i = -50; i <= 50; ++i)
    {
        assert(chain_has(&ch, i) && chain_erase(&ch, i));
        assert(!chain_has(&ch, i));
    }
    chain_destroy(&ch);
    BTreeNode left = {0}, right = {0}, bt = {0};
    left.n = right.n = 2;
    left.leaf = right.leaf = true;
    left.key[0] = 1;
    left.key[1] = 3;
    right.key[0] = 7;
    right.key[1] = 9;
    bt.n = 1;
    bt.key[0] = 5;
    bt.child[0] = &left;
    bt.child[1] = &right;
    int slot;
    assert(btree_find(&bt, 9, &slot) == &right && slot == 1 && !btree_find(&bt, 4, &slot));
    BPlus bl = {0}, br = {0}, bp = {0};
    bl.n = br.n = 3;
    bl.leaf = br.leaf = true;
    bl.key[0] = 1;
    bl.key[1] = 2;
    bl.key[2] = 3;
    br.key[0] = 5;
    br.key[1] = 7;
    br.key[2] = 9;
    bl.next = &br;
    bp.n = 2;
    bp.key[0] = 3;
    bp.key[1] = 9;
    bp.child[0] = &bl;
    bp.child[1] = &br;
    assert(bplus_find(&bp, 3, &slot) == &bl && slot == 2 && !bplus_find(&bp, 4, &slot));
    puts(
        "06_search: PASS (bounds x300, AVL mixed operations x1200, hash model x5000, B/B+ search)");
    return 0;
}
