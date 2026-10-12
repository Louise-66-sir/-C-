/* 408 C revision: 02_stack_queue. Standalone teaching program. */
#include "common.h"

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

/* Self-check entry point. */
int main(void)
{
    Stack s;
    stack_init(&s);
    int x;
    assert(!pop(&s, &x));
    for (int i = 0; i < CAP; ++i)
    {
        assert(push(&s, i));
    }
    assert(!push(&s, 0));
    for (int i = CAP - 1; i >= 0; --i)
    {
        assert(pop(&s, &x) && x == i);
    }
    SharedStack st;
    shared_init(&st);
    for (int i = 0; i < CAP / 2; ++i)
    {
        assert(shared_push(&st, 1, i));
        assert(shared_push(&st, 2, -i));
    }
    assert(!shared_push(&st, 1, 8));
    for (int i = CAP / 2 - 1; i >= 0; --i)
    {
        assert(shared_pop(&st, 1, &x) && x == i);
        assert(shared_pop(&st, 2, &x) && x == -i);
    }
    assert(!shared_pop(&st, 2, &x));
    Queue q;
    queue_init(&q);
    assert(!dequeue(&q, &x));
    for (int round = 0; round < 20; ++round)
    {
        for (int i = 0; i < CAP - 1; ++i)
        {
            assert(enqueue(&q, i));
        }
        assert(queue_size(&q) == CAP - 1 && !enqueue(&q, 9));
        for (int i = 0; i < CAP - 1; ++i)
        {
            assert(dequeue(&q, &x) && x == i);
        }
        assert(queue_size(&q) == 0);
    }
    Cell *top = NULL;
    linked_push(&top, 7);
    assert(linked_pop(&top, &x) && x == 7 && !top);
    assert(!linked_pop(&top, &x));
    LinkedQueue lq;
    lq_init(&lq);
    assert(!lq_dequeue(&lq, &x));
    lq_enqueue(&lq, 8);
    assert(lq_dequeue(&lq, &x) && x == 8 && lq.rear == lq.front);
    lq_enqueue(&lq, 9);
    lq_destroy(&lq);
    assert(brackets_ok("a{[()]}b") && brackets_ok(""));
    assert(!brackets_ok("([)]") && !brackets_ok("(") && !brackets_ok("]"));
    assert(postfix_eval("8 3 2 - /", &x) && x == 8);
    assert(postfix_eval("-12 3 / 2 +", &x) && x == -2);
    assert(!postfix_eval("1 0 /", &x) && !postfix_eval("1 +", &x));
    assert(!postfix_eval("1 2", &x) && !postfix_eval("1+2", &x));
    assert(!postfix_eval("2147483647 1 +", &x) && !postfix_eval("-2147483648 -1 /", &x));
    assert(!postfix_eval("", &x) && !postfix_eval("abc", &x));
    TwoStackQueue ts;
    tsq_init(&ts);
    for (int i = 0; i < CAP; ++i)
    {
        assert(tsq_enqueue(&ts, i));
    }
    assert(!tsq_enqueue(&ts, 1));
    for (int i = 0; i < CAP / 2; ++i)
    {
        assert(tsq_dequeue(&ts, &x) && x == i);
    }
    for (int i = CAP; i < CAP + CAP / 2; ++i)
    {
        assert(tsq_enqueue(&ts, i));
    }
    for (int i = CAP / 2; i < CAP + CAP / 2; ++i)
    {
        assert(tsq_dequeue(&ts, &x) && x == i);
    }
    assert(!tsq_dequeue(&ts, &x));
    assert(symmetric_index(1, 4) == 11 && symmetric_index(4, 1) == 11);
    for (int n = 1; n < 20; ++n)
    {
        int count = 0;
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                int index = tridiagonal_index(n, i, j);
                if (abs(i - j) <= 1)
                {
                    assert(index == count++);
                }
                else
                {
                    assert(index == -1);
                }
            }
        }
        assert(count == (n == 1 ? 1 : 3 * n - 2));
    }
    Triple a[] = {{0, 2, 5}, {1, 0, 7}, {1, 2, 8}}, b[3];
    fast_transpose(a, 3, 3, b);
    assert(b[0].row == 0 && b[0].col == 1 && b[0].value == 7);
    assert(b[1].row == 2 && b[1].col == 0 && b[2].col == 1);
    puts("02_stack_queue: PASS (capacity, wraparound x20, linked structures, arrays)");
    return 0;
}
