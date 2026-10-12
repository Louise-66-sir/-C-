/* 408 C revision: 01_linear. Standalone teaching program. */
#include "common.h"

typedef struct
{
    int data[CAP];
    int n;
} SeqList;

bool seq_insert(SeqList *L, int pos, int x)
{
    if (pos < 0 || pos > L->n || L->n == CAP)
    {
        return false;
    }
    for (int j = L->n; j > pos; --j)
    {
        L->data[j] = L->data[j - 1];
    }
    L->data[pos] = x;
    ++L->n;
    return true;
}

bool seq_erase(SeqList *L, int pos, int *out)
{
    if (pos < 0 || pos >= L->n)
    {
        return false;
    }
    *out = L->data[pos];
    for (int j = pos; j + 1 < L->n; ++j)
    {
        L->data[j] = L->data[j + 1];
    }
    --L->n;
    return true;
}

/* 删除所有 [lo,hi] 内的元素，保持剩余元素相对顺序。 */
bool seq_remove_range(SeqList *L, int lo, int hi)
{
    if (lo > hi)
    {
        return false;
    }
    int k = 0;
    for (int i = 0; i < L->n; ++i)
    {
        if (L->data[i] < lo || L->data[i] > hi)
        {
            L->data[k++] = L->data[i];
        }
    }
    L->n = k;
    return true;
}

/* 前提：非降序。重复值保留第一个。返回新长度。 */
int unique_sorted(int a[], int n)
{
    if (n == 0)
    {
        return 0;
    }
    int k = 1;
    for (int i = 1; i < n; ++i)
    {
        if (a[i] != a[k - 1])
        {
            a[k++] = a[i];
        }
    }
    return k;
}

typedef struct Node
{
    int data;
    struct Node *next;
} Node;

Node *node_new(int x)
{
    Node *p = xmalloc(sizeof *p);
    p->data = x;
    p->next = NULL;
    return p;
}

Node *list_build(const int a[], int n)
{
    Node *head = node_new(0), *tail = head;
    for (int i = 0; i < n; ++i)
    {
        tail->next = node_new(a[i]);
        tail = tail->next;
    }
    return head;
}

void list_destroy(Node *head)
{
    while (head)
    {
        Node *q = head->next;
        free(head);
        head = q;
    }
}

bool list_insert(Node *head, int pos, int x)
{
    if (pos < 0)
    {
        return false;
    }
    Node *pre = head;
    for (int i = 0; i < pos && pre; ++i)
    {
        pre = pre->next;
    }
    if (!pre)
    {
        return false;
    }
    Node *p = node_new(x);
    p->next = pre->next;
    pre->next = p;
    return true;
}

bool list_erase(Node *head, int pos, int *out)
{
    if (pos < 0)
    {
        return false;
    }
    Node *pre = head;
    for (int i = 0; i < pos && pre; ++i)
    {
        pre = pre->next;
    }
    if (!pre || !pre->next)
    {
        return false;
    }
    Node *p = pre->next;
    *out = p->data;
    pre->next = p->next;
    free(p);
    return true;
}

void list_reverse(Node *head)
{
    Node *p = head->next;
    head->next = NULL;
    while (p)
    {
        Node *q = p->next; /* 必须先保存原后继。 */
        p->next = head->next;
        head->next = p;
        p = q;
    }
}

void list_remove_value(Node *head, int x)
{
    Node *pre = head;
    while (pre->next)
    {
        Node *p = pre->next;
        if (p->data == x)
        {
            pre->next = p->next;
            free(p);
        }
        else
        {
            pre = p; /* 删除后不移动 pre，继续检查新后继。 */
        }
    }
}

/* 合并两个升序表，复用结点；消耗 B，释放 B 的头结点。
   A、B 是不同头结点，且有效结点互不共享；返回 A，调用后不得再访问 B。 */
Node *list_merge(Node *A, Node *B)
{
    Node *p = A->next, *q = B->next, *tail = A;
    while (p && q)
    {
        if (p->data <= q->data)
        {
            tail->next = p;
            p = p->next;
        }
        else
        {
            tail->next = q;
            q = q->next;
        }
        tail = tail->next;
    }
    tail->next = p ? p : q;
    free(B);
    return A;
}

/* k 从 1 起；无环表。先让 fast 从首个有效结点走 k 步。 */
Node *kth_from_end(Node *head, int k)
{
    if (k <= 0)
    {
        return NULL;
    }
    Node *fast = head->next, *slow = head->next;
    for (int i = 0; i < k; ++i)
    {
        if (!fast)
        {
            return NULL;
        }
        fast = fast->next;
    }
    while (fast)
    {
        fast = fast->next;
        slow = slow->next;
    }
    return slow;
}

/* 偶数长度返回靠后的中点；无环表。 */
Node *list_middle(Node *head)
{
    Node *slow = head->next, *fast = head->next;
    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/* 入参是首个有效结点，不是头结点。 */
Node *cycle_entry(Node *first)
{
    Node *slow = first, *fast = first;
    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast)
        {
            Node *p = first;
            while (p != slow)
            {
                p = p->next;
                slow = slow->next;
            }
            return p;
        }
    }
    return NULL;
}

/* 两个无环链表的首个公共结点，比较地址；O(m+n)/O(1)。 */
Node *first_common(Node *a, Node *b)
{
    Node *p = a, *q = b;
    while (p != q)
    {
        p = p ? p->next : b;
        q = q ? q->next : a;
    }
    return p;
}

/* 主元素：出现次数严格超过 n/2；候选必须二次验证。 */
bool majority(const int a[], int n, int *out)
{
    int candidate = 0, votes = 0;
    for (int i = 0; i < n; ++i)
    {
        if (votes == 0)
        {
            candidate = a[i];
            votes = 1;
        }
        else
        {
            votes += a[i] == candidate ? 1 : -1;
        }
    }
    int count = 0;
    for (int i = 0; i < n; ++i)
    {
        count += a[i] == candidate;
    }
    if (count <= n / 2)
    {
        return false;
    }
    *out = candidate;
    return true;
}

typedef struct DNode
{
    int data;
    struct DNode *prev, *next;
} DNode;

/* 循环双链表，带头结点 H；空表时 H->next=H->prev=H。 */
void dlist_insert_after(DNode *p, DNode *s)
{
    s->next = p->next;
    s->prev = p;
    p->next->prev = s;
    p->next = s;
}

/* p 是有效结点，不能是头结点。 */
void dlist_erase(DNode *p)
{
    p->prev->next = p->next;
    p->next->prev = p->prev;
    free(p);
}

/* Self-check entry point. */
static void check_list(Node *h, const int a[], int n)
{
    Node *p = h->next;
    for (int i = 0; i < n; ++i)
    {
        assert(p && p->data == a[i]);
        p = p->next;
    }
    assert(!p);
}

int main(void)
{
    SeqList s = {{0}, 0};
    int x;
    assert(!seq_erase(&s, 0, &x));
    assert(!seq_insert(&s, -1, 1));
    for (int i = 0; i < CAP; ++i)
    {
        assert(seq_insert(&s, i, i));
    }
    assert(!seq_insert(&s, CAP, 7));
    assert(seq_erase(&s, 0, &x) && x == 0);
    assert(seq_remove_range(&s, 2, CAP));
    assert(s.n == 1 && s.data[0] == 1);
    assert(!seq_remove_range(&s, 3, 2));
    int a[] = {1, 1, 2, 2, 2, 5};
    assert(unique_sorted(a, 6) == 3);
    assert(a[0] == 1 && a[1] == 2 && a[2] == 5);
    assert(unique_sorted(a, 0) == 0);
    for (int n = 0; n <= 40; ++n)
    {
        int values[40], reversed[40];
        for (int i = 0; i < n; ++i)
        {
            values[i] = i;
            reversed[i] = n - 1 - i;
        }
        Node *h = list_build(values, n);
        check_list(h, values, n);
        assert(!kth_from_end(h, 0) && !kth_from_end(h, n + 1));
        for (int k = 1; k <= n; ++k)
        {
            assert(kth_from_end(h, k)->data == n - k);
        }
        Node *mid = list_middle(h);
        assert(n ? mid->data == n / 2 : mid == NULL);
        assert(cycle_entry(h->next) == NULL);
        list_reverse(h);
        check_list(h, reversed, n);
        list_reverse(h);
        check_list(h, values, n);
        assert(list_insert(h, n, 100));
        assert(list_erase(h, n, &x) && x == 100);
        assert(!list_erase(h, n, &x));
        list_destroy(h);
    }
    int b[] = {2, 2, 2, 3, 2};
    Node *h = list_build(b, 5);
    list_remove_value(h, 2);
    int expect[] = {3};
    check_list(h, expect, 1);
    list_destroy(h);
    int c[] = {1, 3, 5}, d[] = {2, 3, 8}, merged[] = {1, 2, 3, 3, 5, 8};
    h = list_merge(list_build(c, 3), list_build(d, 3));
    check_list(h, merged, 6);
    list_destroy(h);
    h = list_merge(list_build(NULL, 0), list_build(c, 3));
    check_list(h, c, 3);
    list_destroy(h);
    Node *p = node_new(1), *q = node_new(2), *r = node_new(3);
    p->next = q;
    q->next = r;
    r->next = q;
    assert(cycle_entry(p) == q);
    r->next = NULL;
    Node *v = node_new(9);
    v->next = q;
    assert(first_common(p, v) == q);
    v->next = NULL;
    free(v);
    list_destroy(p);
    assert(!first_common(NULL, NULL));
    int maj[] = {2, 1, 2, 2};
    assert(majority(maj, 4, &x) && x == 2);
    assert(!majority(c, 3, &x) && !majority(c, 0, &x));
    DNode *sentinel = xmalloc(sizeof *sentinel), *item = xmalloc(sizeof *item);
    sentinel->next = sentinel->prev = sentinel;
    dlist_insert_after(sentinel, item);
    assert(item->next == sentinel && item->prev == sentinel);
    dlist_erase(item);
    assert(sentinel->next == sentinel && sentinel->prev == sentinel);
    free(sentinel);
    puts("01_linear: PASS (empty/full, lists 0..40, merge, cycles, shared tail, majority)");
    return 0;
}
