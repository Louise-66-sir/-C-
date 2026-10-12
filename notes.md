# 408 数据结构 · C 代码复习笔记

线性结构 → 字符串 → 树 → 图 → 查找 → 排序

**使用目标：看懂代码、手工追踪、独立写出算法，并能说明正确性和复杂度。**

## 00 阅读说明与代码约定

### 00.1 如何使用这份材料

这是一份按 408 常见数据结构知识体系组织的原创复习材料，包含你要求的线性表、栈与队列、串、树与二叉树、查找、排序，以及补充的图、数组与压缩存储。未指定备考年份，因此不将它标注为某年官方考试大纲，也不把“复习优先级”解释为命题概率。

Notebook 以 **Markdown 文本单元中的 C 代码块**呈现，专门用于阅读和做笔记；不需要 C 内核，也不要用 Python 内核执行代码块。HTML 版内容相同，适合浏览器阅读、搜索和打印。想运行时，可使用附带的 C 源码包：每章一个独立程序，有完整头文件、函数与自测入口。

每章依次阅读：**考点地图 → 核心实现 → 过程例题 → 易错点 → 自测与答案**。标记含义：

| 标记 | 复习要求 |
| --- | --- |
| 写 | 看懂后合上材料，能写出结构和核心函数 |
| 推 | 能画图、追踪数组或指针变化、计算代价 |
| 扩 | 综合题迁移、补充实现或进阶理解 |

### 00.2 统一约定

- C 代码参照 Visual Studio Community 常见排版：4 个空格缩进，大括号独占一行，分支与循环使用花括号，每条语句独立一行。
- 使用 **标准 C11**，不使用 C++ 引用、模板、STL 或 `new/delete`。
- 数组、串、图顶点均从 **0** 编号；线性表插入位置也用 0 基下标。题目若按第 1 个元素编号，先减 1。
- 单链表默认**带头结点**，头结点不存有效数据；普通二叉树不带头结点。
- 空二叉树高度为 0，叶结点高度为 1；树高按结点层数计算。
- `bool` 表示操作成功与否；查找返回下标或指针；失败为 `-1` 或 `NULL`。输出参数必须指向有效对象。
- `CAP=256`、`MAXV=64` 是教学示例容量。调用者保证不超过容量；部分关键操作会额外检查。图权值、路径和、Huffman 权和均假定在类型可表示范围内。
- `INF=INT_MAX/4` 表示无边或不可达；合法有限路径值必须小于 `INF`。不把权值 0 当作无边。
- 使用 `xmalloc`：分配失败打印错误并结束程序，使教材代码集中表达算法；考试中应说明内存申请成功的假设。
- 每段代码可能依赖同章前面的类型与函数。源码包已按依赖顺序合并，不能把任意片段当作独立完整程序。
- 未特别检查的参数，如非负长度、合法顶点/叶子下标、有效输出指针与输出容量，由调用者保证。

各章共享头文件中的关键定义如下，源码包另提供完整 `common.h`。

```c
/* 公共约定节选；不是需要单独运行的程序。 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define CAP 256
#define MAXV 64
#define INF (INT_MAX / 4)
```

### 00.3 复杂度与答题表达

时间复杂度计算随输入规模增长的基本操作次数。顺序语句取主导项，嵌套循环应分析**实际循环范围**；递归还要计算调用栈。

| 代码模式 | 时间 | 辅助空间 | 说明 |
| --- | --- | --- | --- |
| 单次扫描长度 n 的表 | O(n) | O(1) | 输入数组不计入辅助空间 |
| i 每次乘 2，直到达到 n | O(log n) | O(1) | 每次翻倍 |
| i=0..n-1，j=0..i | O(n²) | O(1) | 1+2+…+n |
| T(n)=2T(n/2)+O(n) | O(n log n) | 视实现而定 | 归并排序额外数组 O(n) |
| T(n)=T(n-1)+O(1) | O(n) | O(n) | 退化树递归 |
| T(n)=T(n-1)+O(n) | O(n²) | O(n) | 最差快速排序 |

写算法题时按四步组织：①说明数据结构和前提；②说明思路与关键不变量；③给代码；④给时间、辅助空间和边界情况。不能只贴模板而不解释为什么满足题意。

### 00.4 C 语言最常见的扣分点

`Node *p` 是指针；`p->next` 访问所指结点字段。函数需要修改调用者的根指针时，返回新根，或用 `Node **root`。本笔记统一用“返回新根”的风格实现 BST/AVL。

`sizeof p` 是指针大小，申请结点用 `sizeof *p`；结构体赋值会复制字段，指针字段仍指向相同对象。删除时先保存后继再 `free`；释放后不再读取结点。比较写 `==`，赋值写 `=`；数组合法区间 `[0,n)`，闭区间代码另行说明。

## 01 线性表

<!-- module: 01_linear -->

### 01.1 考点地图与存储结构选择

| 内容 | 要达到的能力 | 代价/关键前提 | 标记 |
| --- | --- | --- | --- |
| 顺序表 | 插入、删除、扫描、区间删除、去重 | 随机访问 O(1)，中间插删 O(n) | 写 |
| 单链表 | 建表、定位、插删、逆置、归并 | 定位 O(n)，已知前驱后插删 O(1) | 写 |
| 双链表、循环链表 | 画出指针更新及边界 | 空表的首尾关系需统一 | 写/推 |
| 快慢指针 | 中点、倒数第 k、判环与环入口 | 避免提前访问空指针 | 写 |
| 综合应用 | 公共结点、主元素、最小缺失正整数 | 比较值与比较地址的区别 | 写/扩 |

顺序表适合按下标访问；链表适合频繁改变链接。链表不具备按序号 O(1) 随机访问。静态链表用数组下标模拟指针，插删本质仍是修改链接域，需维护空闲链。

### 01.2 顺序表：插入、删除、稳定压缩

```c
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
```

压缩不变量：处理 `a[i]` 前，`[0,k)` 已保存前面所有应保留的元素；始终有 `k≤i`，因此写入不会破坏尚未读取的元素。两种压缩均 O(n) 时间、O(1) 辅助空间。逐个删除并反复移动会退化到 O(n²)。

### 01.3 链表：先保留链接，再改变链接

```c
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
```

逆置和删除 O(n)/O(1)，归并 O(m+n)/O(1)。插删函数含定位时为 O(n)；不能因为指针赋值只有两三句就称整个函数 O(1)。头插建表会逆序，尾插保持输入顺序。

### 01.4 快慢指针与公共结点

```c
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
```

判环相遇后为什么从起点重新走？设起点到入口长度为 a，入口到相遇点为 b，环长为 L，则快慢路程差是 L 的整数倍，从而 `a+b` 是 L 的整数倍。一个从起点、一个从相遇点同速前进，会在入口相遇。

主元素抵消法是“不同元素两两抵消”；存在主元素时它必为最终候选，**没有主元素时也可能留下候选**。验证不能省略。

### 01.5 双链表与循环链表的指针规则

```c
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
```

单循环链表带尾指针时，头结点为 `rear->next`；删除唯一有效结点后必须把尾指针恢复为头结点。循环链表遍历以“再次到达头结点”为终止条件，不能等待 `NULL`。

### 01.6 过程例题与易错点

**例：**对 `[2,5,3,5,7]` 删除所有 5。读指针 i 依次扫描，写指针 k 依次形成 `[2]→[2,3]→[2,3,7]`，新长度为 3，数组后部残留值无须清零。

**例：**`1→2→3→4→5` 倒数第 2 个。fast 先走两步指向 3；随后同步移动，fast 变 NULL 时 slow 指向 4。

易错：忽略空表；第 pos 个与下标 pos 混淆；逆置时覆盖原 next；删除连续目标时误跳过；归并共享链导致环；两个共享尾部的链表分别销毁导致二次释放。

### 01.7 自测题

1. 写函数对无序顺序表删除所有 x，保持剩余顺序，要求 O(n)/O(1)。
2. 把两个升序链表归并为降序表，复用结点且 O(1) 辅助空间，如何改写？
3. 长度为 n 的数组中，找最小缺失正整数，要求 O(n) 时间，允许 O(n) 辅助空间。
4. 不知道前驱，只知道待删除结点 p，能否总在 O(1) 时间删除它？

### 01.8 参考答案

1. 使用 `k=0`，扫描 i，`a[i]!=x` 时写 `a[k++]=a[i]`，最后 `n=k`。不变量与区间删除相同。
2. 每次取两个表当前较小结点，采用头插法放入结果；剩余结点也逐个头插，不能直接连接剩余升序链。
3. 长度 n 的答案在 `[1,n+1]`。申请 n+1 个布尔标记，遇 `1≤a[i]≤n` 时标记，最后找第一个未出现的 1..n，否则返回 n+1。负数和大于 n 的数忽略。
4. 不能。非尾结点可复制后继数据并删除后继，但改变了结点身份，也不适用于尾结点；若题目强调结点身份或外部引用，此技巧不等价。

## 02 栈、队列、数组与压缩存储

<!-- module: 02_stack_queue -->

### 02.1 考点地图

| 内容 | 必须掌握 | 标记 |
| --- | --- | --- |
| 顺序栈、链栈 | 空满判定、入栈出栈、头指针变化 | 写 |
| 共享栈 | 两栈相向增长，top1+1=top2 为满 | 写/推 |
| 循环队列 | front/rear 含义、空满与长度公式 | 写 |
| 链队列 | 头结点、删除最后一个结点后的 rear | 写 |
| 应用 | 括号、表达式、递归消除、层序/BFS | 写/推 |
| 数组、矩阵 | 行/列优先地址，压缩下标，稀疏三元组 | 写/推 |

栈后进先出，队列先进先出。对于入栈序列 1..n 的合法出栈序列，可用模拟栈验证；元素只能按给定顺序入栈。

### 02.2 顺序栈、共享栈与循环队列

```c
typedef struct
{
    int a[CAP];
    int top;
} Stack;

void stack_init(Stack *s)
{
    s->top = -1;
}

bool push(Stack *s, int x)
{
    if (s->top == CAP - 1)
    {
        return false;
    }
    s->a[++s->top] = x;
    return true;
}

bool pop(Stack *s, int *x)
{
    if (s->top == -1)
    {
        return false;
    }
    *x = s->a[s->top--];
    return true;
}

typedef struct
{
    int a[CAP];
    int top1, top2;
} SharedStack;

void shared_init(SharedStack *s)
{
    s->top1 = -1;
    s->top2 = CAP;
}

bool shared_push(SharedStack *s, int which, int x)
{
    if ((which != 1 && which != 2) || s->top1 + 1 == s->top2)
    {
        return false;
    }
    if (which == 1)
    {
        s->a[++s->top1] = x;
    }
    else
    {
        s->a[--s->top2] = x;
    }
    return true;
}

bool shared_pop(SharedStack *s, int which, int *x)
{
    if (which == 1 && s->top1 >= 0)
    {
        *x = s->a[s->top1--];
        return true;
    }
    if (which == 2 && s->top2 < CAP)
    {
        *x = s->a[s->top2++];
        return true;
    }
    return false;
}

/* front 指向队头，rear 指向下一个入队位置，牺牲一个单元。 */
typedef struct
{
    int a[CAP];
    int front, rear;
} Queue;

void queue_init(Queue *q)
{
    q->front = q->rear = 0;
}

int queue_size(const Queue *q)
{
    return (q->rear - q->front + CAP) % CAP;
}

bool enqueue(Queue *q, int x)
{
    if ((q->rear + 1) % CAP == q->front)
    {
        return false;
    }
    q->a[q->rear] = x;
    q->rear = (q->rear + 1) % CAP;
    return true;
}

bool dequeue(Queue *q, int *x)
{
    if (q->front == q->rear)
    {
        return false;
    }
    *x = q->a[q->front];
    q->front = (q->front + 1) % CAP;
    return true;
}
```

本循环队列最多存 `CAP-1` 个元素。若使用 count 或 tag 区分空满，可存满 CAP 个，但公式需要整体改变，不能混用。

### 02.3 链栈与链队列

```c
typedef struct Cell
{
    int value;
    struct Cell *next;
} Cell;

/* 链栈不带头结点，top=NULL 表示空。 */
void linked_push(Cell **top, int x)
{
    Cell *p = xmalloc(sizeof *p);
    p->value = x;
    p->next = *top;
    *top = p;
}

bool linked_pop(Cell **top, int *x)
{
    if (!*top)
    {
        return false;
    }
    Cell *p = *top;
    *x = p->value;
    *top = p->next;
    free(p);
    return true;
}

typedef struct
{
    Cell *front, *rear;
} LinkedQueue;

void lq_init(LinkedQueue *q)
{
    q->front = q->rear = xmalloc(sizeof *q->front);
    q->front->next = NULL;
}

void lq_enqueue(LinkedQueue *q, int x)
{
    Cell *p = xmalloc(sizeof *p);
    p->value = x;
    p->next = NULL;
    q->rear->next = p;
    q->rear = p;
}

bool lq_dequeue(LinkedQueue *q, int *x)
{
    Cell *p = q->front->next;
    if (!p)
    {
        return false;
    }
    *x = p->value;
    q->front->next = p->next;
    if (q->rear == p)
    {
        q->rear = q->front;
    }
    free(p);
    return true;
}

void lq_destroy(LinkedQueue *q)
{
    int x;
    while (lq_dequeue(q, &x))
    {
    }
    free(q->front);
    q->front = q->rear = NULL;
}
```

### 02.4 括号匹配与两个栈模拟队列

```c
bool brackets_ok(const char *s)
{
    Stack st;
    stack_init(&st);
    for (int i = 0; s[i]; ++i)
    {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{')
        {
            if (!push(&st, c))
            {
                return false;
            }
        }
        else if (c == ')' || c == ']' || c == '}')
        {
            int left;
            if (!pop(&st, &left))
            {
                return false;
            }
            if (!((left == '(' && c == ')') || (left == '[' && c == ']') ||
                  (left == '{' && c == '}')))
            {
                return false;
            }
        }
    }
    return st.top == -1;
}

/* 本例总容量限制为 CAP，避免 out 尚有数据时 in 被塞满。 */
typedef struct
{
    Stack in, out;
    int count;
} TwoStackQueue;

void tsq_init(TwoStackQueue *q)
{
    stack_init(&q->in);
    stack_init(&q->out);
    q->count = 0;
}

bool tsq_enqueue(TwoStackQueue *q, int x)
{
    if (q->count == CAP)
    {
        return false;
    }
    if (!push(&q->in, x))
    {
        return false;
    }
    ++q->count;
    return true;
}

bool tsq_dequeue(TwoStackQueue *q, int *x)
{
    if (q->count == 0)
    {
        return false;
    }
    if (q->out.top == -1)
    {
        int t;
        while (pop(&q->in, &t))
        {
            push(&q->out, t);
        }
    }
    pop(&q->out, x);
    --q->count;
    return true;
}
```

两个栈模拟队列只在 out 为空时搬运。每个元素最多进入 in、搬到 out、离开 out 各一次，所以单次出队最差 O(n)，**一串操作的均摊代价**为每次 O(1)。不能写“每次出队最坏 O(1)”。

表达式处理：中缀转后缀时，操作数直接输出，左括号入栈，右括号弹到左括号，运算符按优先级和结合性弹栈。后缀求值时先弹出的是右操作数，后弹出的是左操作数。此处只讨论普通二元运算；一元负号需要额外词法/语法规则。

下面给出后缀求值完整函数，依赖公共头文件的 `<ctype.h>` 和 `<errno.h>`。输入 token 以空白分隔，支持带符号十进制整数及 `+ - * /`；使用 C 的整数除法，非整数实数表达式不在本例范围。

```c
bool postfix_eval(const char *text, int *answer)
{
    Stack st;
    stack_init(&st);
    const char *p = text;
    while (*p)
    {
        while (isspace((unsigned char)*p))
        {
            ++p;
        }
        if (!*p)
        {
            break;
        }
        bool op = (*p == '+' || *p == '-' || *p == '*' || *p == '/') &&
                  (p[1] == '\0' || isspace((unsigned char)p[1]));
        if (op)
        {
            int right, left;
            if (!pop(&st, &right) || !pop(&st, &left))
            {
                return false;
            }
            long long value;
            if (*p == '+')
            {
                value = (long long)left + right;
            }
            else if (*p == '-')
            {
                value = (long long)left - right;
            }
            else if (*p == '*')
            {
                value = (long long)left * right;
            }
            else
            {
                if (right == 0)
                {
                    return false;
                }
                value = (long long)left / right;
            }
            if (value < INT_MIN || value > INT_MAX || !push(&st, (int)value))
            {
                return false;
            }
            ++p;
        }
        else
        {
            char *end;
            errno = 0;
            long value = strtol(p, &end, 10);
            if (end == p || errno == ERANGE || value < INT_MIN || value > INT_MAX ||
                (*end && !isspace((unsigned char)*end)))
            {
                return false;
            }
            if (!push(&st, (int)value))
            {
                return false;
            }
            p = end;
        }
    }
    if (st.top != 0)
    {
        return false;
    }
    return pop(&st, answer);
}
```

每个 token 只处理一次，时间 O(输入字符数)，栈空间 O(token数)。本实现还检查了操作数不足、除零、非法 token、结果越界和最终栈中存在多余操作数。

### 02.5 多维数组与特殊矩阵

二维数组 `A[m][n]`，元素宽 w，基地址是 `A[0][0]`：行优先地址 `base+(i*n+j)*w`；列优先地址 `base+(j*m+i)*w`。C 的内建多维数组采用行优先。

| 对象 | 0 基压缩规则 | 长度 |
| --- | --- | --- |
| 对称矩阵，下三角含主对角 | i≥j 时 k=i(i+1)/2+j，否则交换 i,j | n(n+1)/2 |
| 下三角矩阵 | i≥j 同上；i<j 访问公共常量槽 | n(n+1)/2+1 |
| 上三角，按行存含对角 | i≤j 时 k=i*n-i(i-1)/2+(j-i) | n(n+1)/2+1（另有常量槽） |
| 三对角矩阵，按行压缩 | 合法带内元素 k=2i+j | n≥2 时 3n-2；n=1 时 1 |

公式来源要能推：先数前面完整行的元素数，再加当前行偏移。对称矩阵上下三角存储是**二选一**；三角矩阵的另一半是同一常量。三对角矩阵只允许 `|i-j|≤1`。

```c
int symmetric_index(int i, int j)
{
    if (i < j)
    {
        int t = i;
        i = j;
        j = t;
    }
    return i * (i + 1) / 2 + j;
}

int tridiagonal_index(int n, int i, int j)
{
    if (i < 0 || i >= n || j < 0 || j >= n || abs(i - j) > 1)
    {
        return -1;
    }
    return 2 * i + j;
}

/* 稀疏矩阵三元组：输入按行有序，转置后仍按行有序。
   列数 cols<=CAP，非零元数 nnz<=CAP；行列从0开始。 */
typedef struct
{
    int row, col, value;
} Triple;

void fast_transpose(const Triple a[], int nnz, int cols, Triple b[])
{
    int count[CAP] = {0}, start[CAP];
    for (int k = 0; k < nnz; ++k)
    {
        ++count[a[k].col];
    }
    int sum = 0;
    for (int c = 0; c < cols; ++c)
    {
        start[c] = sum;
        sum += count[c];
    }
    for (int k = 0; k < nnz; ++k)
    {
        int pos = start[a[k].col]++;
        b[pos] = (Triple){a[k].col, a[k].row, a[k].value};
    }
}
```

快速转置 O(cols+nnz) 时间、O(cols) 辅助空间，不含输出数组。稀疏三元组存储不支持一般意义上的按位置 O(1) 随机访问。

### 02.6 例题、自测与答案

**例：**循环队列容量 8，front=6，rear=2，长度 `(2-6+8)%8=4`，下次入队写下标 2，再令 rear=3。

**例：**后缀 `8 3 2 - /`：先算 `3-2=1`，再算 `8/1=8`。不能把弹出次序写成 `2-3`。

**自测：**①入栈 1,2,3，能否出栈 3,1,2？②CAP=8，front=3，rear=2，是否满？③5 阶对称矩阵存下三角，A[1][4] 在哪？④两个栈队列是否每次都必须搬运？

**答案：**①不能，3 出栈后栈顶为 2。②满，`(2+1)%8=3`，长度 7。③交换为 A[4][1]，k=4×5/2+1=11。④只在 out 为空时搬运，否则会破坏先进先出顺序。

## 03 串与模式匹配

<!-- module: 03_string -->

### 03.1 考点地图与定义

串的长度是字符数；空串长度 0，空格串仍含字符。子串是连续片段，子序列可不连续。C 字符串以 `'\0'` 结尾，本章按**字节**比较，不按 Unicode 汉字个数比较。模式匹配在整个 408 知识体系中也常归到查找；这里独立成章便于复习。

| 内容 | 要求 | 时间 |
| --- | --- | --- |
| 朴素匹配 BF | 起点移动，主串/模式位置关系 | 最坏 O(nm) |
| KMP | 前后缀含义，构造 pi，匹配时仅回退模式 | O(n+m) |
| next / nextval | 与教材下标约定对应，手算回退 | 构造 O(m) |
| 串操作 | 长度、比较、子串、连接及容量边界 | 按操作分析 |

### 03.2 朴素匹配

```c
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
```

每次失配将候选起点增加 1；最坏例子是主串大量 a、模式为若干 a 后跟 b。不能把 BF 最坏复杂度写成 O(n+m)。

### 03.3 KMP：统一采用前缀函数 pi

`pi[i]` 表示 `p[0..i]` 的最长相等**真前缀与真后缀**长度。“真”表示不能使用整个串自身。pi 存的是长度，取值 0..i。

```c
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
```

匹配不变量：处理主串 `s[i]` 前，模式前 j 个字符与已处理主串的最后 j 个字符相同。失配后 `j=pi[j-1]` 保留最长可继续利用的边界，i 不后退。j 的上升和下降总次数受线性约束；预处理 O(m)，匹配 O(n)，辅助空间 O(m)。

### 03.4 教材 next / nextval 如何对应

常见教材使用 1 基模式串，`next[1]=0`，j=0 是特殊状态。与本章 pi 的关系为：**j≥2 时 `next[j]=pi[j-2]+1`**。这只适用于上述 next 定义，不能把任何名为 next 的数组都机械转换。

本节另给 **0 基、next[0]=-1** 的 nextval 构造；失配回退索引也必须使用相同版本。它不能直接替代上面 `pi[j-1]` 的表达式。

```c
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
```

nextval 的作用：若回退后的模式字符仍等于刚失配的模式字符，就继续回退，省去必定失败的比较。并不改变 KMP 的渐近复杂度。

### 03.5 手算例题、自测与答案

**例：**模式 `ababaca`：

| i | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| p[i] | a | b | a | b | a | c | a |
| pi[i] | 0 | 0 | 1 | 2 | 3 | 0 | 1 |
| 1 基 next[i+1] | 0 | 1 | 1 | 2 | 3 | 4 | 1 |

到 i=5 的 c 时，j 从 3 回退到 pi[2]=1，再到 pi[0]=0，仍不匹配，因此 pi[5]=0。先区分“已经匹配了几个字符”与“下次比较哪个下标”。

**自测：**①`aaaaa` 中 `aaa` 出现几次？②匹配成功后找全部重叠位置，j 如何更新？③空模式的位置如何约定？④主串匹配下标 i 在 KMP 中能否回退？

**答案：**①3 次，起点 0、1、2。②`j=pi[m-1]`，不清零。③本章首次匹配返回 0，计数为 n+1；题目若另有规定以题意为准。④不回退，失配调整 j。

## 04 树与二叉树

<!-- module: 04_tree -->

### 04.1 考点地图与基本性质

| 内容 | 要达到的能力 | 标记 |
| --- | --- | --- |
| 二叉链表、顺序存储 | 编号、孩子/双亲位置、空指针数 | 写/推 |
| 遍历 | 先中后序递归、非递归、层序 | 写 |
| 递归应用 | 高度、叶子数、镜像、祖先、平衡判定 | 写 |
| 完全二叉树 | 层序判定、最后非叶结点、结点数性质 | 写/推 |
| 线索二叉树 | ltag/rtag、构造、前驱后继 | 写/推 |
| 树与森林 | 双亲/孩子/孩子兄弟表示，转换与遍历 | 推 |
| Huffman | 合并过程、WPL、编码、前缀性质 | 写/推 |
| 并查集与堆 | 合并查找，堆调整 | 写；实现见图/排序 |

二叉树允许左右子树为空，左右有序；它不是任意“度≤2的树”的同义词。非空二叉树 `n0=n2+1`，n 个结点的二叉链表有 `n+1` 个空孩子指针。高度 h 的二叉树最多 `2^h-1` 个结点；有 n 个结点的完全二叉树高度为 `⌊log2 n⌋+1`。

完全二叉树按 0 基数组存储：左孩子 `2i+1`，右孩子 `2i+2`，非根结点双亲 `(i-1)/2`；孩子下标必须 `<n`。一般二叉树顺序存储会浪费空间。

### 04.2 递归模板：空树、分解、合并

```c
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
```

上述递归函数通常访问每个结点常数次，时间 O(n)、调用栈 O(h)；最坏链状树 h=n。逐个结点调用 `tree_height` 再判平衡会最坏 O(n²)，`balanced_height` 一次后序扫描即 O(n)。普通二叉树 LCA 不能按值大小选择左右分支；那是 BST 才有的性质。

### 04.3 非递归遍历、层序与完全性

以下显式栈和队列要求结点总数≤CAP，输出数组容量≥结点数。

```c
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
```

后序非递归中 `last` 是最后处理完的结点：右子树未处理时先转入右子树，否则访问根。完全性判定的含义是层序出现空位后，后面不允许再出现有效结点。层序队列 O(n) 辅助空间；若循环复用队列单元，实际可按最大层宽优化。

### 04.4 由遍历序列重建

```c
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
```

先序确定根，中序确定左右范围。此实现最坏 O(n²)，辅助递归栈 O(h)；预建元素到中序位置的索引可降低查找代价。**先序+中序、后序+中序**在元素互异时能唯一重建；先序+后序通常不能，单孩子到底在左还是右不确定。

### 04.5 中序线索化

```c
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
```

构造 O(n) 时间，递归 O(h) 空间；线索遍历总 O(n)、额外 O(1)。后继若是右线索直接返回；若有右子树，进入右子树找最左结点。不要把线索当孩子递归，否则可能成环。此处无头结点，最后一个结点右线索为 NULL，与带头结点的教材模板边界不同。

### 04.6 树、森林与孩子兄弟表示

普通树可用双亲数组、孩子链表、孩子兄弟表示。孩子兄弟结点的 `first_child` 对应二叉树左孩子，`next_sibling` 对应右孩子；同一棵普通树的根没有右兄弟，森林多个根通过右兄弟链接。

| 原对象遍历 | 对应二叉表示遍历 |
| --- | --- |
| 普通树先根遍历 | 先序 |
| 普通树后根遍历 | 中序 |
| 森林先序遍历 | 先序 |
| 森林中序遍历 | 中序 |

这里森林中序采用“先第一棵树的子森林，再根，再其余森林”的教材约定。普通树高度不能直接使用二叉树高度函数，因为兄弟链接并不增加普通树层数。

```c
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
```

### 04.7 Huffman 树与编码

n 个正权叶结点，每次取当前最小两个权合并，直到剩一个根。n≥2 时共 2n-1 个结点，所有内部结点度为 2。WPL 为叶权×根到叶的**边数**之和，也等于每次合并得到的权之和。相同权值选择可能不同，所以树形、编码未必唯一，最优 WPL 相同。

```c
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
```

该构建为 O(n²) 时间、O(n) 存储；使用最小堆可达 O(n log n)。单叶树按深度定义 WPL=0，实际发送单一符号时通常约定编码 0；要区分数学树深度和实际协议的编码约定。

### 04.8 例题、自测与答案

**例：**先序 `A B D E C F`，中序 `D B E A C F`：根 A，左子树先序 BDE/中序 DBE，右子树先序 CF/中序 CF。后序为 `D E B F C A`，高度 3，叶子数 3。这棵树不是完全二叉树，因为 C 缺左孩子却有右孩子 F。

**例：**权值 2,3,7,9：合并 2+3=5，5+7=12，9+12=21，WPL=5+12+21=38。

**自测：**①树有 8 个度为 2 的结点，有多少叶子？②为什么中序线索遍历不能无条件走 left？③两个给定结点之一不在树中，原 LCA 代码是否能验证？④如何写第 k 个先序结点？

**答案：**①非空二叉树叶子数 9。②left 可能是前驱线索，应检查 ltag。③不能，它假设两点存在；应先查验存在性，或递归返回“已找到个数+候选”。④用先序计数，每访问一个有效结点计数加 1，达到 k 时保存并停止；题目 k 通常从 1 起，空树和 k 越界返回失败。最坏 O(n)，递归栈 O(h)。

## 05 图：存储、遍历与算法

<!-- module: 05_graph -->

### 05.1 考点地图与算法选型

| 问题 | 算法 | 前提 | 本章实现时间 |
| --- | --- | --- | --- |
| 可达性、连通分量 | DFS/BFS | 有向图需区分强弱连通 | 邻接表 O(V+E) |
| 无权最短路 | BFS | 每条边代价相同 | O(V+E) |
| 最小生成树 | Prim | 无向带权连通图 | 矩阵版 O(V²) |
| 最小生成树 | Kruskal+并查集 | 无向图，边按权排序 | O(E log E) |
| 单源最短路 | Dijkstra | 边权非负 | 矩阵版 O(V²) |
| 全源最短路 | Floyd | 允许负边，最短路不受负环影响 | O(V³) |
| 先后依赖、判有向环 | 拓扑排序 | DAG 才能输出全部顶点 | O(V+E) |
| AOE 工期与关键活动 | 拓扑+逆拓扑 | DAG，活动持续时间非负 | O(V+E) |

MST 最小化连通所有顶点的**总边权**；最短路最小化源到目标的路径代价。MST 允许负边。Dijkstra 不允许负边，不能因为“没有负环”就使用它。Floyd 的负环检测见后文。

### 05.2 邻接表、邻接矩阵与特殊链式结构

```c
typedef struct Arc
{
    int to, weight;
    struct Arc *next;
} Arc;

typedef struct
{
    int n;
    bool directed;
    Arc *head[MAXV];
} Graph;

void graph_init(Graph *g, int n, bool directed)
{
    assert(n >= 0 && n <= MAXV);
    g->n = n;
    g->directed = directed;
    for (int i = 0; i < n; ++i)
    {
        g->head[i] = NULL;
    }
}

void arc_add(Graph *g, int u, int v, int w)
{
    Arc *p = xmalloc(sizeof *p);
    p->to = v;
    p->weight = w;
    p->next = g->head[u];
    g->head[u] = p;
}

/* 顶点下标合法；教学示例默认简单图，不输入重复边和自环。 */
void edge_add(Graph *g, int u, int v, int w)
{
    assert(u >= 0 && u < g->n && v >= 0 && v < g->n);
    arc_add(g, u, v, w);
    if (!g->directed)
    {
        arc_add(g, v, u, w);
    }
}

void graph_destroy(Graph *g)
{
    for (int i = 0; i < g->n; ++i)
    {
        Arc *p = g->head[i];
        while (p)
        {
            Arc *q = p->next;
            free(p);
            p = q;
        }
        g->head[i] = NULL;
    }
}

typedef struct
{
    int n;
    int w[MAXV][MAXV];
} Matrix;

void to_matrix(const Graph *g, Matrix *m)
{
    m->n = g->n;
    for (int i = 0; i < g->n; ++i)
    {
        for (int j = 0; j < g->n; ++j)
        {
            m->w[i][j] = i == j ? 0 : INF;
        }
    }
    for (int u = 0; u < g->n; ++u)
    {
        for (Arc *p = g->head[u]; p; p = p->next)
        {
            if (p->weight < m->w[u][p->to])
            {
                m->w[u][p->to] = p->weight;
            }
        }
    }
}
```

邻接矩阵空间 O(V²)，判断两点间是否有边 O(1)；邻接表空间 O(V+E)，枚举某顶点出边 O(outdegree)。无向图一条边通常对应两个弧结点。邻接表头插会使遍历次序与输入次序相反；DFS/BFS 顺序不唯一，题目给出按编号访问时必须按要求排序。

十字链表用于有向图，一个弧结点含尾/头顶点、同尾下一弧、同头下一弧，可遍历出边和入边；邻接多重表用于无向图，一个边结点由两个端点共享。阅读代码时重点认出“边结点是否共享”和“下一链接按哪个端点走”，删除要同步维护两边链接。本材料给结构识读，未提供这两类结构的完整增删程序。

### 05.3 DFS、BFS 与路径恢复

```c
void dfs_visit(const Graph *g, int u, bool seen[], int out[], int *k)
{
    seen[u] = true;
    out[(*k)++] = u;
    for (Arc *p = g->head[u]; p; p = p->next)
    {
        if (!seen[p->to])
        {
            dfs_visit(g, p->to, seen, out, k);
        }
    }
}

/* 返回无向图连通分量个数；有向图此计数不表示强连通分量。 */
int dfs_all(const Graph *g, int out[])
{
    bool seen[MAXV] = {false};
    int k = 0, components = 0;
    for (int u = 0; u < g->n; ++u)
    {
        if (!seen[u])
        {
            ++components;
            dfs_visit(g, u, seen, out, &k);
        }
    }
    return components;
}

void bfs_distance(const Graph *g, int source, int dist[], int parent[])
{
    int q[MAXV], f = 0, r = 0;
    for (int i = 0; i < g->n; ++i)
    {
        dist[i] = -1;
        parent[i] = -1;
    }
    dist[source] = 0;
    q[r++] = source;
    while (f < r)
    {
        int u = q[f++];
        for (Arc *p = g->head[u]; p; p = p->next)
        {
            if (dist[p->to] == -1)
            {
                dist[p->to] = dist[u] + 1;
                parent[p->to] = u;
                q[r++] = p->to; /* 入队时标记，避免重复入队。 */
            }
        }
    }
}

/* parent形成源点为根的前驱树，输出source到target；不可达返回0。 */
int restore_path(int source, int target, const int parent[], int path[])
{
    int n = 0, u = target;
    while (u != -1 && n < MAXV)
    {
        path[n++] = u;
        if (u == source)
        {
            break;
        }
        u = parent[u];
    }
    if (n == 0 || path[n - 1] != source)
    {
        return 0;
    }
    for (int i = 0; i < n / 2; ++i)
    {
        swap_int(&path[i], &path[n - 1 - i]);
    }
    return n;
}
```

无向图从一个源点开始遍历只能覆盖其连通分量，遍历全部顶点要有外层循环。DFS 调用栈最坏 O(V)，BFS 队列 O(V)。邻接矩阵扫描每个顶点整行，DFS/BFS 时间变成 O(V²)。

无向图 DFS 判环时忽略父边；有向图不能这样判，应使用未访问/递归栈内/已完成三色状态，遇到指向递归栈内顶点的边有环，或通过拓扑输出数量判断。强连通分量算法属于额外拓展，此处不作完整实现。

### 05.4 并查集、Kruskal 与 Prim

```c
typedef struct
{
    int parent[MAXV];
} DSU;

void dsu_init(DSU *d, int n)
{
    for (int i = 0; i < n; ++i)
    {
        d->parent[i] = -1;
    }
}

/* 根处parent为负集合大小；路径压缩。 */
int dsu_find(DSU *d, int x)
{
    if (d->parent[x] < 0)
    {
        return x;
    }
    return d->parent[x] = dsu_find(d, d->parent[x]);
}

bool dsu_union(DSU *d, int a, int b)
{
    a = dsu_find(d, a);
    b = dsu_find(d, b);
    if (a == b)
    {
        return false;
    }
    if (d->parent[a] > d->parent[b])
    {
        swap_int(&a, &b);
    }
    d->parent[a] += d->parent[b];
    d->parent[b] = a;
    return true;
}

typedef struct
{
    int u, v, w;
} Edge;

int edge_cmp(const void *a, const void *b)
{
    int x = ((const Edge *)a)->w, y = ((const Edge *)b)->w;
    return (x > y) - (x < y); /* 不用x-y，避免减法溢出。 */
}

/* edges每条无向边仅出现一次；就地排序。连通返回true，total为MST权和。 */
bool kruskal(int n, Edge edges[], int e, int *total)
{
    DSU d;
    dsu_init(&d, n);
    qsort(edges, (size_t)e, sizeof *edges, edge_cmp);
    int count = 0;
    *total = 0;
    for (int i = 0; i < e && count < n - 1; ++i)
    {
        if (dsu_union(&d, edges[i].u, edges[i].v))
        {
            ++count;
            *total += edges[i].w;
        }
    }
    return n == 0 || count == n - 1;
}

/* 非空无向图；失败表示不连通，total此时只是已选部分。 */
bool prim(const Matrix *g, int *total)
{
    if (g->n == 0)
    {
        *total = 0;
        return true;
    }
    bool used[MAXV] = {false};
    int low[MAXV];
    for (int i = 0; i < g->n; ++i)
    {
        low[i] = INF;
    }
    low[0] = 0;
    *total = 0;
    for (int k = 0; k < g->n; ++k)
    {
        int u = -1;
        for (int v = 0; v < g->n; ++v)
        {
            if (!used[v] && (u == -1 || low[v] < low[u]))
            {
                u = v;
            }
        }
        if (u == -1 || low[u] == INF)
        {
            return false;
        }
        used[u] = true;
        *total += low[u];
        for (int v = 0; v < g->n; ++v)
        {
            if (!used[v] && g->w[u][v] < low[v])
            {
                low[v] = g->w[u][v];
            }
        }
    }
    return true;
}
```

Prim 的 low[v] 是 v 到**已选顶点集合**的最小边权，更新式是 `low[v]=w[u][v]`；Dijkstra 的 dist[v] 是源到 v 的路径长度，更新式要加 dist[u]。并查集合并按集合大小、查找做路径压缩，操作序列的均摊复杂度接近 O(1)，严谨写 O(α(V))；不把单次最坏等同均摊。

### 05.5 Dijkstra 与 Floyd

```c
/* 前提：边权非负；source合法。parent用于恢复路径。 */
void dijkstra(const Matrix *g, int source, int dist[], int parent[])
{
    bool done[MAXV] = {false};
    for (int i = 0; i < g->n; ++i)
    {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[source] = 0;
    for (int k = 0; k < g->n; ++k)
    {
        int u = -1;
        for (int v = 0; v < g->n; ++v)
        {
            if (!done[v] && (u == -1 || dist[v] < dist[u]))
            {
                u = v;
            }
        }
        if (u == -1 || dist[u] == INF)
        {
            break;
        }
        done[u] = true;
        for (int v = 0; v < g->n; ++v)
        {
            if (!done[v] && g->w[u][v] != INF && dist[u] + g->w[u][v] < dist[v])
            {
                dist[v] = dist[u] + g->w[u][v];
                parent[v] = u;
            }
        }
    }
}

/* 原地更新矩阵。返回false表示存在负环，此时不能解释为普通最短路。 */
bool floyd(Matrix *g)
{
    for (int k = 0; k < g->n; ++k)
    {
        for (int i = 0; i < g->n; ++i)
        {
            for (int j = 0; j < g->n; ++j)
            {
                if (g->w[i][k] != INF && g->w[k][j] != INF && g->w[i][k] + g->w[k][j] < g->w[i][j])
                {
                    g->w[i][j] = g->w[i][k] + g->w[k][j];
                }
            }
        }
    }
    for (int i = 0; i < g->n; ++i)
    {
        if (g->w[i][i] < 0)
        {
            return false;
        }
    }
    return true;
}
```

Floyd 不变量：第 k 轮后，允许编号≤k 的顶点作为中间点。**k 必须最外层**。无穷值不参与加法；本教学实现还假定算法中的有限权和均不溢出。若负环可达，受影响点对没有有限最短路，不能只看到某个负数结果就当答案。

### 05.6 拓扑排序与关键路径

```c
/* 有向图；order输出拓扑序；只用局部入度，不破坏图。 */
bool topo_sort(const Graph *g, int order[])
{
    int degree[MAXV] = {0}, q[MAXV], f = 0, r = 0, count = 0;
    for (int u = 0; u < g->n; ++u)
    {
        for (Arc *p = g->head[u]; p; p = p->next)
        {
            ++degree[p->to];
        }
    }
    for (int u = 0; u < g->n; ++u)
    {
        if (degree[u] == 0)
        {
            q[r++] = u;
        }
    }
    while (f < r)
    {
        int u = q[f++];
        order[count++] = u;
        for (Arc *p = g->head[u]; p; p = p->next)
        {
            if (--degree[p->to] == 0)
            {
                q[r++] = p->to;
            }
        }
    }
    return count == g->n;
}

/* 非负权DAG；多源/多汇等价于增加持续时间0的超级源/汇。
   工期为所有汇点最早时间最大值；vl以统一工期初始化。 */
bool critical_times(const Graph *g, int ve[], int vl[], int *duration)
{
    int order[MAXV];
    if (!topo_sort(g, order))
    {
        return false;
    }
    for (int i = 0; i < g->n; ++i)
    {
        ve[i] = 0;
    }
    for (int k = 0; k < g->n; ++k)
    {
        int u = order[k];
        for (Arc *p = g->head[u]; p; p = p->next)
        {
            if (ve[u] + p->weight > ve[p->to])
            {
                ve[p->to] = ve[u] + p->weight;
            }
        }
    }
    *duration = 0;
    for (int u = 0; u < g->n; ++u)
    {
        if (!g->head[u] && ve[u] > *duration)
        {
            *duration = ve[u];
        }
    }
    for (int u = 0; u < g->n; ++u)
    {
        vl[u] = *duration;
    }
    for (int k = g->n - 1; k >= 0; --k)
    {
        int u = order[k];
        for (Arc *p = g->head[u]; p; p = p->next)
        {
            if (vl[p->to] - p->weight < vl[u])
            {
                vl[u] = vl[p->to] - p->weight;
            }
        }
    }
    return true;
}

/* 活动u->v的最早开始ve[u]，最迟开始vl[v]-w；相等则为关键活动。 */
```

拓扑排序 O(V+E)，结果通常不唯一；出现多个当前零入度点意味着可有多种选择。关键路径是持续时间最长的源汇路径，可能不唯一。缩短一条关键活动不一定缩短总工期，其他关键路径可能保持原工期；非关键活动延长在其时差以内通常不改变工期。

### 05.7 过程例题

**无向带权图：**边 0-1:2、0-2:5、1-2:1、1-3:4、2-3:2。Kruskal 依次选 1-2:1、0-1:2、2-3:2，总权 5。0 到各点最短距离为 `[0,2,3,5]`，到 3 的路径为 0→1→2→3。BFS 若忽略权值，到 3 只需 2 条边；它与权值最短路不是同一问题。

**AOE：**0→1:3、0→2:2、1→3:2、2→3:4。ve=`[0,3,2,6]`，vl=`[0,4,2,6]`，工期 6，关键活动是 0→2、2→3。0→1 的最早开始 0，最迟开始 1，时差 1。

### 05.8 自测与参考答案

1. BFS 为什么要在入队时标记？答：否则多个前驱会把同一顶点重复入队，破坏“一点一次”的队列容量与代价分析。
2. Prim 能否使用边权 0 或负权？答：能；用 used 标记是否入树，不能用 low==0 代替。图必须无向，完整 MST 要求连通。
3. 有向图 0→1:-2、1→2:3、0→2:2，能否用本章 Dijkstra？答：不满足非负权前提，应改用适合负权的算法，如 Bellman-Ford，或 DAG 的拓扑松弛。
4. 拓扑只输出 V-1 个顶点说明什么？答：存在有向环，剩余顶点不一定全部位于环上，也可能只是被环上的依赖阻塞。
5. 图不连通时 Kruskal 的已选边是什么？答：最小生成森林的一部分/结果，选边数为 V-c（c 为分量数）；不能报告为完整 MST。上述函数返回 false，输出权和只表示已选森林。

## 06 查找

<!-- module: 06_search -->

### 06.1 查找方法、成功失败与 ASL

| 方法 | 条件 | 查找时间 | 关键考点 |
| --- | --- | --- | --- |
| 顺序查找 | 可无序 | O(n) | 哨兵、成功/失败比较次数 |
| 折半查找 | 有序、可随机访问 | O(log n) | 区间定义、边界、判定树 |
| 分块查找 | 块间有序，块内可无序 | 依索引与块大小 | 先定位块再顺序查找 |
| BST | 左小右大 | O(h)，最坏 O(n) | 删除、退化树 |
| AVL、红黑树 | 平衡约束 | O(log n) | 旋转、性质与维护规则 |
| B/B+树 | 多路平衡 | O(log n)量级，重点看磁盘层数 | 分裂、借位、合并、叶链 |
| 散列 | 良好散列与合适装填因子 | 期望 O(1)，最坏 O(n) | 冲突、探测、删除标记 |

ASL 是按查找概率加权的比较次数均值：成功 `Σp_i*C_i`，失败按各失败位置的概率单独计算。等概率成功顺序查找 ASL=(n+1)/2；折半查找应画实际判定树计数，**不能一律把成功 ASL 写成 log2 n**。

### 06.2 顺序、折半与边界查找

```c
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
```

非降序数组中 key 的个数为 `upper_bound-lower_bound`；查首个 key 时，lower 返回的位置还要检查 `<n && a[pos]==key`。哨兵顺序查找需要额外/可覆盖的存储槽；不应直接覆盖有效元素却不恢复。

分块若顺序查索引，b 块、块长 s，等概率成功平均比较约为 `(b+1)/2+(s+1)/2`（假设各块等长等概率），n=bs 时 s 取约√n 可平衡两段代价；若索引用折半，则代价模型变成 O(log b+s)，不能照搬同一最优块长结论。

### 06.3 BST：查找、插入与三类删除

```c
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
```

删除：叶结点直接断开；一个孩子时孩子顶替；两个孩子时用中序前驱或后继替换关键字，再删除替身。若结点还有记录字段，需要一起复制；本例只存 key。BST 中序有序，但高度不保证对数，递增序列插入会形成链。

### 06.4 AVL：高度、四种失衡与插入

```c
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
```

平衡因子定义为左高-右高。LL：右旋；RR：左旋；LR：先左旋左孩子，再右旋根；RL：先右旋右孩子，再左旋根。旋转保持中序顺序。插入 O(log n)，递归空间 O(log n)；**删除维护需沿祖先继续检查，不能直接套用按插入 key 判断失衡的代码**。

删除先做 BST 删除，再更新高度、按**孩子当前平衡因子**选择旋转。一次删除可能使多个祖先失衡，需要沿递归返回路径持续维护。

```c
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
```

孩子平衡因子为 0 的情况会在删除中出现，此时选择单旋。删除时间与递归空间均 O(log n)。调用者同样需要 `root=avl_erase(root,key)`。

### 06.5 红黑树：性质与插入过程追踪

红黑树是满足以下约束的 BST：结点红或黑；根黑；空叶 NIL 黑；红结点的孩子都黑；从任一结点到后代 NIL 的各条路径包含相同数量的黑结点。计黑高时应先说明是否包含起点和 NIL，并全程一致。按边数计算的树高满足 `h≤2log2(n+1)`。

插入的结构化流程：先按 BST 插入**红结点**，再修复。若父黑直接结束；若父红且叔红，把父叔变黑、祖父变红，继续向上；若父红叔黑/NIL，按内侧/外侧情况先旋转到外侧，再旋转祖父并交换父祖颜色；最后根改黑。左右情况镜像对称。

**过程例：**依次插 10、5、1：10 为黑根；5 红；1 红导致父红叔黑，是外侧 LL，于 10 右旋并重染色，结果 5 黑根、1 和 10 红。插 15 时父 10 与叔 1 均红，先变父叔黑、祖父 5 红，最后根 5 改黑。

删除要追踪被实际移除结点的原颜色及黑高缺口；红结点移除通常无需黑高修复，黑结点移除可能需要借助兄弟、侄结点旋转重染色。本材料以性质和过程判断为目标，**未提供红黑树完整插删程序**，不应把 AVL 的四个分支当作红黑树修复代码。

用 x 表示承担黑高缺口的替代位置。若 x 在父结点左侧，兄弟 w 在右侧，可按下表追踪；右侧情形左右镜像。近侄是 w 的左孩子，远侄是 w 的右孩子，NIL 视为黑。

| 情况 | 调整与后续 |
| --- | --- |
| x 为红 | 把 x 变黑，吸收缺口，结束 |
| 兄弟红 | 兄弟变黑、父变红、左旋父，转换为黑兄弟情况 |
| 兄弟黑、两个侄子均黑 | 兄弟变红，缺口上移到父；父原为红时可染黑结束 |
| 兄弟黑、近侄红、远侄黑 | 近侄染黑、兄弟染红、右旋兄弟，转换成远侄红 |
| 兄弟黑、远侄红 | 兄弟继承父颜色，父与远侄染黑，左旋父，结束 |

这些是经典红黑树删除修复的状态规则，具体代码还要统一哨兵 NIL、双亲指针与根更新，不能省略这些约定后机械翻译。

### 06.6 B 树与 B+ 树：规则与查找代码

本节采用 **m 阶 B 树=每个结点最多 m 个孩子** 的教材约定。一个内部结点若有 k 个关键字就有 k+1 个孩子；非根至少 `⌈m/2⌉-1` 个关键字，至多 m-1 个；非叶根至少 2 个孩子；所有叶结点在同一层。空树根可为空，单结点树另算。

插入先到应在的叶子插入关键字；超过 m-1 后分裂，把中间关键字上移，左右各保留一部分；父结点也可能连锁分裂，根分裂会增高。删除内部关键字可换前驱/后继；删除后下溢，优先向足够多关键字的相邻兄弟借，并通过父关键字转移；兄弟不能借则合并，父可能继续下溢，根无键时高度下降。

**3 阶 B 树例：**依次插 10、20、30，叶 `[10,20,30]` 溢出，中键 20 上升成为根，孩子为 `[10]`、`[30]`。再插 40、50，右叶溢出，中键 40 上升，根为 `[20,40]`，孩子 `[10] [30] [50]`。删除 30 时左右兄弟都处于最少键数，将父中键 20 下移，与左兄弟合并成 `[10,20]`，根余 `[40]`，另一孩子 `[50]`。

```c
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
```

B+ 树的记录集中在叶结点，叶结点有序链接，适合区间扫描。教材对“阶”“内部索引键数量”“分隔键取最大/最小”的定义可能不同，先认题目约定。**本节 B+ 查找采用内部每个孩子配一个该子树最大键的约定**，即有 k 个孩子就存 k 个最大键；与上面的 B 树“k 键 k+1 孩子”不同。

```c
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
```

B+ 插入叶记录，分裂后叶中仍保留实际记录，向父层更新索引；不能像 B 树那样把记录本身从叶子永久移走。删除也可能借位/合并并更新父索引及叶链。以上只提供 B/B+ **查找函数**及增删过程讲解；没有完整的分裂合并程序。查磁盘块时重点数访问层数，结点内顺序扫描/二分的 CPU 代价另计。

### 06.7 散列表：线性探测与墓碑

```c
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
```

删除不直接标 EMPTY，否则同一探测链后续键会被错误判不存在。查找遇 DELETED 继续；遇 EMPTY 可停；整表最多探测 HM 次，避免死循环。墓碑过多会降低性能，需要重建表。装填因子 α=有效元素数/槽数；查找代价还受探测策略、键分布与墓碑影响。

线性探测会产生聚集。二次探测、双散列需说明公式和能否遍历足够槽位；双散列步长与表长互素可遍历全表。拉链法把同槽键组织成链表，装填因子允许大于 1。

```c
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
```

### 06.8 例题、自测与答案

**例：**数组 `[1,2,2,2,5]` 查 2，lower=1，upper=4，出现 3 次。查 4，lower=4，但 a[4]=5，因此不存在。

**例：**表长 7，H(key)=key%7，线性探测插 10、17、24，占槽 3、4、5；删 17 后槽 4 设为墓碑，查 24 仍须检查 3、4、5。失败查找按具体探测轨迹计比较，终止空槽是否计一次按题目定义说明。

**自测：**①AVL 插 30、10、20 是哪类？②BST 删除根后为何必须接收返回值？③B+ 内部索引出现 key 就能停止吗？④散列表期望 O(1) 是否代表任何输入都 O(1)？

**答案：**①LR，先对 10 左旋，再对 30 右旋，20 成根。②根可能释放并被孩子替换，旧指针不再有效。③不能，记录在叶结点。④不是，碰撞严重时可退化到 O(n)。

## 07 排序

<!-- module: 07_sort -->

### 07.1 比较表：先记性质，再记代码

稳定表示相等关键字记录的原有相对顺序不改变；整数数组看不出相等记录的身份，手算时应写成 `3a、3b`。下面性质针对**本章写法**，复杂度以 n≥2 讨论。

| 算法 | 最好时间 | 平均时间 | 最坏时间 | 辅助空间 | 稳定 |
| --- | --- | --- | --- | --- | --- |
| 直接插入 | O(n) | O(n²) | O(n²) | O(1) | 是 |
| 折半插入 | O(n log n)比较 | O(n²)总体 | O(n²)总体 | O(1) | 是 |
| 冒泡（提前结束） | O(n) | O(n²) | O(n²) | O(1) | 是 |
| 简单选择 | O(n²) | O(n²) | O(n²) | O(1) | 否 |
| 希尔（折半增量） | 依数据与增量 | 依增量序列 | O(n²)上界 | O(1) | 否 |
| 快速（首元枢轴） | O(n log n) | O(n log n)，随机排列模型 | O(n²) | 平均 O(log n)，最坏 O(n)栈 | 否 |
| 堆排序 | 通常教材计 O(n log n) | O(n log n) | O(n log n) | O(1) | 否 |
| 二路归并 | O(n log n) | O(n log n) | O(n log n) | O(n)含辅助数组 | 是 |
| LSD 基数 | O(d(n+r)) | 同左 | 同左 | O(n+r) | 是 |

堆排序本章遇父≥子会提前停止下滤，**全相等输入可为 O(n)**；教材表常在互异关键字/一般比较模型下列最好 O(n log n)，答题以题目计量前提为准。希尔排序不存在脱离增量序列的统一平均 O(n log n) 结论。基数 r 是桶数，d 是处理位数，不能隐藏 d 而无条件写 O(n)。

### 07.2 直接插入与折半插入

```c
void insertion_sort(int a[], int n)
{
    for (int i = 1; i < n; ++i)
    {
        int x = a[i], j = i - 1;
        while (j >= 0 && a[j] > x)
        {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = x;
    }
}

void binary_insertion_sort(int a[], int n)
{
    for (int i = 1; i < n; ++i)
    {
        int x = a[i], low = 0, high = i;
        /* 找第一个>x的位置，把新记录放在旧的相等记录之后。 */
        while (low < high)
        {
            int mid = low + (high - low) / 2;
            if (a[mid] <= x)
            {
                low = mid + 1;
            }
            else
            {
                high = mid;
            }
        }
        for (int j = i; j > low; --j)
        {
            a[j] = a[j - 1];
        }
        a[low] = x;
    }
}
```

循环开始时 `[0,i)` 已有序，将 a[i] 插入正确位置后 `[0,i+1)` 有序。稳定性来自只移动**严格大于** x 的记录。折半减少比较，不减少移动；本版本没有“已有序就跳过二分”的优化，所以有序输入比较仍是 Θ(n log n)。

### 07.3 冒泡、简单选择与希尔

```c
void bubble_sort(int a[], int n)
{
    for (int end = n - 1; end > 0; --end)
    {
        bool changed = false;
        for (int j = 0; j < end; ++j)
        {
            if (a[j] > a[j + 1])
            {
                swap_int(&a[j], &a[j + 1]);
                changed = true;
            }
        }
        if (!changed)
        {
            break;
        }
    }
}

void selection_sort(int a[], int n)
{
    for (int i = 0; i + 1 < n; ++i)
    {
        int smallest = i;
        for (int j = i + 1; j < n; ++j)
        {
            if (a[j] < a[smallest])
            {
                smallest = j;
            }
        }
        if (smallest != i)
        {
            swap_int(&a[i], &a[smallest]);
        }
    }
}

void shell_sort(int a[], int n)
{
    for (int gap = n / 2; gap > 0; gap /= 2)
    {
        for (int i = gap; i < n; ++i)
        {
            int x = a[i], j = i;
            while (j >= gap && a[j - gap] > x)
            {
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = x;
        }
    }
}
```

冒泡每趟确定当前最大值的最终位置；选择每趟确定当前最小值的最终位置，但**长距离交换**可能越过相同关键字。希尔只是使各间隔子序列有序，gap>1 的一趟结束后全表未必有序。

### 07.4 快速排序与第 k 小

```c
/* 闭区间[lo,hi]，首元枢轴，挖坑式双向划分。 */
int partition_c(int a[], int lo, int hi)
{
    int pivot = a[lo];
    while (lo < hi)
    {
        while (lo < hi && a[hi] >= pivot)
        {
            --hi;
        }
        a[lo] = a[hi];
        while (lo < hi && a[lo] <= pivot)
        {
            ++lo;
        }
        a[hi] = a[lo];
    }
    a[lo] = pivot;
    return lo;
}

void quick_sort(int a[], int lo, int hi)
{
    if (lo >= hi)
    {
        return;
    }
    int p = partition_c(a, lo, hi);
    quick_sort(a, lo, p - 1);
    quick_sort(a, p + 1, hi);
}

/* k为0基次序，0<=k<n；会重排数组。平均O(n)，最坏O(n²)。 */
bool quick_select(int a[], int n, int k, int *out)
{
    if (k < 0 || k >= n)
    {
        return false;
    }
    int lo = 0, hi = n - 1;
    while (lo <= hi)
    {
        int p = partition_c(a, lo, hi);
        if (p == k)
        {
            *out = a[p];
            return true;
        }
        if (p < k)
        {
            lo = p + 1;
        }
        else
        {
            hi = p - 1;
        }
    }
    return false;
}
```

一趟划分只保证枢轴左边≤它、右边≥它，两边内部未必有序。本版本全相等、有序、逆序输入可能连续产生极端划分，最坏 O(n²)。随机枢轴或三数取中能改善典型表现；大量重复值时三路划分更合适，但不能据此声称普通快排最坏 O(n log n)。

### 07.5 堆排序：先建堆，再逐个取最大

```c
/* 大根堆，下标0；n是当前堆的元素数量。 */
void sift_down(int a[], int n, int root)
{
    int x = a[root], child = 2 * root + 1;
    while (child < n)
    {
        if (child + 1 < n && a[child + 1] > a[child])
        {
            ++child;
        }
        if (x >= a[child])
        {
            break;
        }
        a[root] = a[child];
        root = child;
        child = 2 * root + 1;
    }
    a[root] = x;
}

void heap_sort(int a[], int n)
{
    for (int i = n / 2 - 1; i >= 0; --i)
    {
        sift_down(a, n, i);
    }
    for (int end = n - 1; end > 0; --end)
    {
        swap_int(&a[0], &a[end]);
        sift_down(a, end, 0);
    }
}
```

0 基最后一个非叶结点为 `n/2-1`。自底向上建堆 O(n)，不是把每个结点的 O(log n) 简单相乘；多数结点接近叶子，下滤距离很短。升序排序用大根堆；最大元素交换到尾部后**尾部不再属于堆**，因此下滤传 end，而不是 n。

### 07.6 二路归并与逆序对

```c
void merge_range(int a[], int tmp[], int lo, int mid, int hi)
{
    int i = lo, j = mid, k = lo; /* 两段均半开：[lo,mid)、[mid,hi)。 */
    while (i < mid && j < hi)
    {
        tmp[k++] = a[i] <= a[j] ? a[i++] : a[j++];
    }
    while (i < mid)
    {
        tmp[k++] = a[i++];
    }
    while (j < hi)
    {
        tmp[k++] = a[j++];
    }
    for (i = lo; i < hi; ++i)
    {
        a[i] = tmp[i];
    }
}

void merge_rec(int a[], int tmp[], int lo, int hi)
{
    if (hi - lo < 2)
    {
        return;
    }
    int mid = lo + (hi - lo) / 2;
    merge_rec(a, tmp, lo, mid);
    merge_rec(a, tmp, mid, hi);
    merge_range(a, tmp, lo, mid, hi);
}

void merge_sort(int a[], int n)
{
    if (n < 2)
    {
        return;
    }
    int *tmp = xmalloc((size_t)n * sizeof *tmp);
    merge_rec(a, tmp, 0, n);
    free(tmp);
}

/* 逆序对定义i<j且a[i]>a[j]；返回数量，顺带把数组排好序。 */
long long inversion_rec(int a[], int tmp[], int lo, int hi)
{
    if (hi - lo < 2)
    {
        return 0;
    }
    int mid = lo + (hi - lo) / 2;
    long long count = inversion_rec(a, tmp, lo, mid) + inversion_rec(a, tmp, mid, hi);
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi)
    {
        if (a[i] <= a[j])
        {
            tmp[k++] = a[i++];
        }
        else
        {
            count += mid - i;
            tmp[k++] = a[j++];
        }
    }
    while (i < mid)
    {
        tmp[k++] = a[i++];
    }
    while (j < hi)
    {
        tmp[k++] = a[j++];
    }
    for (i = lo; i < hi; ++i)
    {
        a[i] = tmp[i];
    }
    return count;
}

long long inversion_count(int a[], int n)
{
    if (n < 2)
    {
        return 0;
    }
    int *tmp = xmalloc((size_t)n * sizeof *tmp);
    long long result = inversion_rec(a, tmp, 0, n);
    free(tmp);
    return result;
}
```

稳定性来自相等时先取左段。逆序对中右段元素比左段当前值小时，与左段剩下的 `mid-i` 个元素都构成逆序。逆序对最多 n(n-1)/2，用较宽整数保存。

### 07.7 LSD 基数排序

本实现只接受**非负 int**，按十进制从低位到高位；每趟用稳定计数分配。负数不能直接套用取模下标。

```c
bool radix_sort(int a[], int n)
{
    int max = 0;
    for (int i = 0; i < n; ++i)
    {
        if (a[i] < 0)
        {
            return false;
        }
        if (a[i] > max)
        {
            max = a[i];
        }
    }
    if (n < 2)
    {
        return true;
    }
    int *tmp = xmalloc((size_t)n * sizeof *tmp);
    for (int exp = 1;;)
    {
        int count[10] = {0};
        for (int i = 0; i < n; ++i)
        {
            ++count[(a[i] / exp) % 10];
        }
        for (int d = 1; d < 10; ++d)
        {
            count[d] += count[d - 1];
        }
        for (int i = n - 1; i >= 0; --i)
        {
            int d = (a[i] / exp) % 10;
            tmp[--count[d]] = a[i];
        }
        memcpy(a, tmp, (size_t)n * sizeof *a);
        if (max / exp < 10)
        {
            break; /* 先判终止，再乘10，避免exp溢出。 */
        }
        exp *= 10;
    }
    free(tmp);
    return true;
}
```

每趟必须稳定，否则前面低位已形成的相对顺序会被破坏。LSD 从低到高逐趟处理整个序列，MSD 从高位分组后递归处理各组，不要混同。

### 07.8 外部排序：能追踪的流程与内存模拟

外部排序的数据放不进内存。典型流程：读取内存容纳的一块记录、内部排序写成初始有序段；再多路归并，重复到剩一段。m 个初始段，k 路归并的归并趟数约为 `⌈log_k m⌉`，可逐次做 `m=ceil(m/k)` 精确计数；k≥2。

每趟归并通常将所有记录读写一遍；按块计 I/O，若数据占 B 块，均匀全量归并模型每趟约 2B 次块传输，初始段生成的读写另外算。内存通常至少需 k 个输入缓冲区和 1 个输出缓冲区。增加归并路数可减少趟数，但缓冲空间与选最小的开销也会变化。

置换选择用“工作区最小记录”输出，新读记录若小于刚输出值，冻结到下一段；随机数据的平均初始段长度约为工作区容量的 2 倍，**不是所有输入都保证 2 倍**。败者树在每次更换某一路头元素后沿路径调整，选取代价 O(log k)。最优归并树用各段长度作权，按 Huffman 思路使读写代价最小；k 路时必要时补零权虚段，使叶子数满足 `(m-1)%(k-1)==0`。

下面是 k 路归并的**内存模拟**，用于理解“每次取各路当前最小值”。它没有磁盘缓冲、败者树与置换选择，时间 O(Nk)，不是外部排序的完整工程实现。

```c
int multiway_merge(const int *runs[], const int len[], int k, int out[])
{
    int pos[CAP] = {0}, count = 0; /* 0<=k<=CAP，out容量>=各段长度总和。 */
    for (;;)
    {
        int best = -1;
        for (int i = 0; i < k; ++i)
        {
            if (pos[i] < len[i] && (best == -1 || runs[i][pos[i]] < runs[best][pos[best]]))
            {
                best = i;
            }
        }
        if (best == -1)
        {
            break;
        }
        out[count++] = runs[best][pos[best]++];
    }
    return count;
}
```

### 07.9 手算例题与算法选择

数组 `[5,2,4,1,3]`：直接插入第一趟为 `[2,5,4,1,3]`；冒泡从左往右第一趟为 `[2,4,1,3,5]`；选择第一趟为 `[1,2,4,5,3]`。判断算法时要知道“趟”的定义与扫描方向，不能仅看某一位置变有序。

稳定性反例：选择排序 `[2a,2b,1]` 第一趟交换后为 `[1,2b,2a]`，相同键顺序被颠倒。快排、堆排也可能跨位置移动相同记录。

| 场景 | 合理考虑 | 理由/条件 |
| --- | --- | --- |
| 基本有序、规模小 | 直接插入 | 移动少，可接近线性 |
| 要稳定且最坏有保证 | 归并 | O(n log n)，但要额外空间 |
| 原地且最坏有保证 | 堆 | 不稳定 |
| 内存数组一般比较排序 | 改进快排 | 平均表现好，注意最坏退化 |
| 仅找第 k 小 | 快速选择 | 不必排完整个数组 |
| 只需最大的 k 个且k较小 | 容量k的小根堆 | 扫描 O(n log k)，辅助O(k) |
| 位数固定的非负整数 | 基数 | 不受比较排序Ω(n log n)下界约束 |

比较排序下界 Ω(n log n) 针对一般比较模型，不能用来否定计数/基数排序。链表缺少随机访问，直接套数组折半、希尔或堆排序模板不合适；归并很适合链表链接操作。

### 07.10 自测与参考答案

1. 折半插入减少了什么？答：定位的比较次数；最坏移动仍 O(n²)。
2. 初始已经有序，首元枢轴快排怎样？答：可能退化，每轮只排除一个枢轴，T(n)=T(n-1)+O(n)。
3. 归并相等时取右侧，还稳定吗？答：可能不稳定，来自右段的同键记录会越过左段原本在前的同键记录。
4. `[3,1,2]` 的逆序对有几对？答：2 对，(3,1)、(3,2)。
5. 17 个初始段做 4 路归并需要几趟？答：17→5→2→1，共3趟；不把初始段生成混入归并趟数。
6. 一趟快排能确定哪些最终位置？答：至少确定该趟枢轴的一个合法最终位置；两边内部仍可无序，重复键下“第几个相同记录”无唯一身份次序。

## 08 综合算法题：从条件推导代码

<!-- module: 08_exercises -->

### 08.1 独立练习的使用方式

以下题目为**原创练习，不标注为历年真题**。先看题目写出思路与复杂度，再看后面的答案代码。与前文的小型自测合用，覆盖线性结构、树、图与排序的代码迁移。

| 题号 | 题目 | 目标 |
| --- | --- | --- |
| E01 | 原地循环左移数组 k 位 | 三次逆置，O(n)/O(1) |
| E02 | 最小缺失正整数 | 标记范围证明，O(n)/O(n) |
| E03 | 顺序表奇数在前、偶数在后 | 双指针，不要求稳定 |
| E04 | 链表 L0,L1,…,Ln 改排 L0,Ln,L1,Ln-1,… | 中点+逆置+交织，O(n)/O(1) |
| E05 | 判断链表回文，并恢复链表 | 比较半链，O(n)/O(1) |
| E06 | 二叉树带权路径长度 | 叶权×深度，O(n)/O(h) |
| E07 | 按先序找第 k 个结点 | 引用计数状态与提前返回 |
| E08 | 根到叶路径和等于 target 的路径数 | 递归携带剩余值 |
| E09 | 无向简单图是否为树 | 连通+E=V-1 |
| E10 | 输出有向图 s→t 的全部简单路径 | DFS回溯，路径内标记 |
| E11 | 两个升序数组的公共不同值 | 归并双指针 |
| E12 | 未排序数组中最小的 k 个数 | 容量k的大根堆 |

### 08.2 E01—E03：数组的三种扫描

```c
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
```

E01 的证明：将数组分成 A、B 两段，目标 BA；`reverse(reverse(A) reverse(B))=BA`。例 `[1,2,3,4,5]` 左移 2 得 `[3,4,5,1,2]`。E02 n 个数不可能覆盖 1..n+1，所以答案最多 n+1。E03 负奇数在 C 中 `%2` 可能为 -1，因此判断非零，不用 `==1`。

### 08.3 E04—E05：链表中点与半链逆置

本练习章另定义独立结点类型，源码能单独编译；它不依赖第01章文件。链表仍带头结点。

```c
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
```

E04 `[0,1,2,3,4]` 变 `[0,4,1,3,2]`，偶数长度 `[0,1,2,3]` 变 `[0,3,1,2]`。找到的是前半段末尾；断开后右段长度不超过左段，所以交织不会提前耗尽 left。E05 只比较右半段长度次，奇数长度的中间结点不必比较。

### 08.4 E06—E08：树的递归状态

```c
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
```

E06 根深度按**边数**从0开始，不能调用高度约定中的第1层直接乘权；只统计叶子。E07 每遇到一个先序结点扣减目标，命中后沿调用栈传回指针；k 越界返回 NULL。E08 题目要求根到叶，不能在内部结点恰好等于 target 时提前算一条；若允许负权，也不能仅因剩余 target<0 就剪枝。

### 08.5 E09—E10：图的全局标记与路径标记

```c
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
```

E09 仅 E=V-1 不够，比如“三角形加一个孤立点”也满足该式，仍需连通。矩阵版时间 O(V²)，邻接表版可 O(V+E)。E10 初始 on_path 全 false，depth=0；输出规模可能指数甚至阶乘级，不能写 O(V+E)，辅助递归/路径空间 O(V)。普通可达性 DFS 用永久 visited，全部简单路径用路径内 on_path，并在回溯时撤销。

### 08.6 E11—E12：双指针与 Top-k

```c
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
```

E11 每步至少移动一个指针，O(n+m) 时间，不含输出空间为 O(1)。E12 保留 k 个候选中的最大值在堆顶，新值更小才替换；不变量是堆内始终保存当前已扫描元素中最小的 k 个，时间 O(n log k)（k=1 时 O(n)），辅助 O(k)。要最大 k 个则换成小根堆；若题目还要求有序，最后另对结果排序。

## 09 复习清单与验证说明

### 09.1 建议的三轮使用顺序

第一轮按章节阅读，画出存储结构，追踪一个正常输入和一个空/单元素输入。第二轮隐藏代码，从题意写出不变量和函数，再对照查漏。第三轮混合练习，看到题目先判断结构、是否有序、是否需要稳定、能否修改输入以及允许的辅助空间。

| 自查项 | 过关标准 |
| --- | --- |
| 链表 | 逆置、连续删除、归并、倒数k、判环可以独立写 |
| 栈队列 | 能说明每个指针含义及空满条件，能追踪回绕 |
| 数组矩阵 | 能从行长度求和推压缩下标 |
| KMP | 能手算pi，说明失配时j为何回退到pi[j-1] |
| 二叉树 | 递归、显式栈、层序互相转换，区分高度和深度 |
| 线索与森林 | 先看tag再走指针，认清兄弟与孩子 |
| 图 | 先确认有向/无向、带权/无权、负边、连通与DAG |
| 查找树 | BST删除三种情形，AVL四旋转，红黑/B树操作规则 |
| 散列 | 成功/失败探测轨迹、墓碑与装填因子 |
| 排序 | 稳定性反例、一趟结果、复杂度与空间条件 |
| 综合题 | 先写前提和不变量，再写代码，最后分析边界 |

### 09.2 源码包与可运行性

源码包包括 `common.h` 和 `01_linear.c` 到 `08_exercises.c`，每个 .c 都是独立程序，不能把多个含 main 的文件一次链接成一个程序。头文件与源文件放在同一文件夹。

若以后想运行，GCC/Clang 示例：`gcc -std=c11 -O2 01_linear.c -o linear`，再执行生成的程序。Windows 的 Visual Studio 开发者终端可使用：`cl /std:c11 /utf-8 /W4 01_linear.c`，再运行 `01_linear.exe`。这些步骤是可选的，直接阅读 Notebook/HTML 不需要编译器。

每章 main 包含断言自测和一条结果提示；通过时打印 `PASS`。自测覆盖常规和部分边界输入，并对排序、字符串匹配、图算法做交叉验证。实际执行环境、结果和测试范围写在源码包的 `验证说明.md`。测试不能证明全部输入正确；证明依靠前提、不变量和推导，示例容量与整数范围约定仍然适用。

### 09.3 覆盖深度与边界

提供完整教学实现的部分：顺序表/链表核心操作、栈与队列、后缀表达式求值、压缩下标及快速转置、BF/KMP、树遍历与应用、线索化、Huffman、图存储与遍历、并查集、Prim/Kruskal、Dijkstra/Floyd、拓扑与关键时间、顺序/折半/分块、BST、AVL插删、散列、主要内部排序、综合题答案。

提供规则与过程讲解而未给完整维护程序的部分：静态链表、十字链表/邻接多重表、中缀表达式解析与转换、红黑树插删、B/B+树分裂合并、外部排序的真实文件I/O、置换选择和败者树维护。B/B+树另有可编译的查找函数，外部归并另有内存模拟。这样可以明确“已给完整代码”和“需要掌握规则”的区别，不把片段包装成完整系统。

### 09.4 查阅资料

本材料的讲解、例题、练习与 C 实现为重新编写，未复制教材正文或现成代码。以下是用于核对算法定义和适用条件的公开课程资料；它们不是 408 官方考试大纲。

- [MIT 6.006：课程讲义目录](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/pages/lecture-notes/)：查阅排序、散列、AVL、堆、BFS/DFS和最短路课程主题。
- [MIT：AVL 讲义](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/resources/mit6_006s20_lec7/)：对应06.4的平衡与旋转规则。
- [MIT：BFS 讲义](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/resources/mit6_006s20_lec9/)：对应05.3的无权最短路。
- [MIT：Dijkstra 讲义](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/resources/mit6_006s20_lec13/)：对应05.5的非负权前提与贪心选择。
- [Virginia Tech OpenDSA：课程模块索引](https://opendsa-server.cs.vt.edu/embed)：可进一步查看模式匹配、排序、图与并查集的交互演示。

对具体考试年份的范围和术语，以当年正式大纲、指定教材及题目给出的定义为准。本材料适合作为代码复习笔记，不能替代全部概念题、手算题与真题训练。
