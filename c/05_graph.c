/* 408 C revision: 05_graph. Standalone teaching program. */
#include "common.h"

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

/* Self-check entry point. */
static void test_random_graphs(void)
{
    srand(408);
    for (int trial = 0; trial < 200; ++trial)
    {
        int n = 2 + rand() % 11;
        Graph g;
        graph_init(&g, n, false);
        Edge edges[CAP];
        int e = 0;
        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 1; j < n; ++j)
            {
                if (j == i + 1 || rand() % 3 == 0)
                {
                    int w = rand() % 21;
                    edge_add(&g, i, j, w);
                    edges[e++] = (Edge){i, j, w};
                }
            }
        }
        Matrix m, f;
        to_matrix(&g, &m);
        f = m;
        assert(floyd(&f));
        int total1 = 0, total2 = 0;
        assert(prim(&m, &total1) && kruskal(n, edges, e, &total2));
        assert(total1 == total2);
        for (int source = 0; source < n; ++source)
        {
            int dist[MAXV], parent[MAXV], path[MAXV];
            dijkstra(&m, source, dist, parent);
            for (int v = 0; v < n; ++v)
            {
                assert(dist[v] == f.w[source][v]);
                int size = restore_path(source, v, parent, path), sum = 0;
                assert(size > 0);
                for (int k = 1; k < size; ++k)
                {
                    sum += m.w[path[k - 1]][path[k]];
                }
                assert(sum == dist[v]);
            }
        }
        graph_destroy(&g);
    }
}

int main(void)
{
    Graph g;
    graph_init(&g, 4, false);
    Edge edges[] = {{0, 1, 2}, {0, 2, 5}, {1, 2, 1}, {1, 3, 4}, {2, 3, 2}};
    for (int i = 0; i < 5; ++i)
    {
        edge_add(&g, edges[i].u, edges[i].v, edges[i].w);
    }
    Matrix m;
    to_matrix(&g, &m);
    int t, dist[MAXV], par[MAXV], out[MAXV];
    assert(prim(&m, &t) && t == 5);
    assert(kruskal(4, edges, 5, &t) && t == 5);
    dijkstra(&m, 0, dist, par);
    assert(dist[3] == 5 && dist[2] == 3);
    bfs_distance(&g, 0, dist, par);
    assert(dist[3] == 2 && dfs_all(&g, out) == 1);
    graph_destroy(&g);
    graph_init(&g, 3, false);
    edge_add(&g, 0, 1, -4);
    to_matrix(&g, &m);
    assert(!prim(&m, &t) && dfs_all(&g, out) == 2);
    Edge neg[] = {{0, 1, -4}};
    assert(!kruskal(3, neg, 1, &t) && t == -4);
    graph_destroy(&g);
    graph_init(&g, 3, true);
    edge_add(&g, 0, 1, 0);
    to_matrix(&g, &m);
    dijkstra(&m, 0, dist, par);
    assert(dist[1] == 0 && dist[2] == INF);
    assert(restore_path(0, 2, par, out) == 0);
    graph_destroy(&g);
    graph_init(&g, 4, true);
    edge_add(&g, 0, 1, 3);
    edge_add(&g, 0, 2, 2);
    edge_add(&g, 1, 3, 2);
    edge_add(&g, 2, 3, 4);
    int ve[MAXV], vl[MAXV];
    assert(topo_sort(&g, out));
    assert(critical_times(&g, ve, vl, &t) && t == 6);
    int expected_ve[] = {0, 3, 2, 6}, expected_vl[] = {0, 4, 2, 6};
    for (int i = 0; i < 4; ++i)
    {
        assert(ve[i] == expected_ve[i] && vl[i] == expected_vl[i]);
    }
    edge_add(&g, 3, 0, 1);
    assert(!topo_sort(&g, out) && !critical_times(&g, ve, vl, &t));
    graph_destroy(&g);
    graph_init(&g, 5, true);
    edge_add(&g, 0, 1, 2);
    edge_add(&g, 2, 3, 5);
    assert(critical_times(&g, ve, vl, &t) && t == 5 && vl[1] == 5 && vl[0] == 3);
    graph_destroy(&g);
    graph_init(&g, 3, true);
    edge_add(&g, 0, 1, -2);
    edge_add(&g, 1, 2, 3);
    to_matrix(&g, &m);
    assert(floyd(&m) && m.w[0][2] == 1);
    edge_add(&g, 2, 0, -2);
    to_matrix(&g, &m);
    assert(!floyd(&m));
    graph_destroy(&g);
    DSU d;
    dsu_init(&d, 4);
    assert(dsu_union(&d, 0, 1) && dsu_union(&d, 1, 2));
    assert(!dsu_union(&d, 2, 0) && d.parent[dsu_find(&d, 2)] == -3);
    test_random_graphs();
    puts("05_graph: PASS (200 random connected graphs: Prim=Kruskal, Dijkstra=Floyd)");
    return 0;
}
