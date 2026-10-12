/* 408 C revision: 07_sort. Standalone teaching program. */
#include "common.h"

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

/* Self-check entry point. */
static int sort_cmp_test(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static void quick_wrapper(int a[], int n)
{
    quick_sort(a, 0, n - 1);
}

static void test_sorts(const int a[], int n)
{
    void (*sorts[])(int[],
                    int) = {insertion_sort, binary_insertion_sort, bubble_sort, selection_sort,
                            shell_sort,     quick_wrapper,         heap_sort,   merge_sort};
    int ref[CAP], got[CAP];
    memcpy(ref, a, (size_t)n * sizeof *a);
    qsort(ref, (size_t)n, sizeof *ref, sort_cmp_test);
    for (int f = 0; f < 8; ++f)
    {
        memcpy(got, a, (size_t)n * sizeof *a);
        sorts[f](got, n);
        for (int i = 0; i < n; ++i)
        {
            assert(got[i] == ref[i]);
        }
    }
    long long brute = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            brute += a[i] > a[j];
        }
    }
    memcpy(got, a, (size_t)n * sizeof *a);
    assert(inversion_count(got, n) == brute);
    for (int k = 0; k < n; ++k)
    {
        int out;
        memcpy(got, a, (size_t)n * sizeof *a);
        assert(quick_select(got, n, k, &out) && out == ref[k]);
    }
}

int main(void)
{
    srand(408);
    int a[CAP], ref[CAP];
    for (int trial = 0; trial < 600; ++trial)
    {
        int n = trial % 81;
        for (int i = 0; i < n; ++i)
        {
            a[i] = rand() % 101 - 50;
        }
        test_sorts(a, n);
        for (int i = 0; i < n; ++i)
        {
            a[i] = trial % 3 == 0 ? i : trial % 3 == 1 ? n - i : 7;
        }
        test_sorts(a, n);
        for (int i = 0; i < n; ++i)
        {
            a[i] = (rand() % 32768) * 32768 + rand() % 32768;
        }
        memcpy(ref, a, (size_t)n * sizeof *a);
        qsort(ref, (size_t)n, sizeof *ref, sort_cmp_test);
        assert(radix_sort(a, n));
        for (int i = 0; i < n; ++i)
        {
            assert(a[i] == ref[i]);
        }
    }
    int limits[] = {INT_MAX, INT_MIN, 0, -1, 1, INT_MAX, INT_MIN};
    test_sorts(limits, 7);
    int rad[] = {INT_MAX, 0, 10, 1, INT_MAX};
    assert(radix_sort(rad, 5) && rad[4] == INT_MAX);
    int bad[] = {-1, 0};
    assert(!radix_sort(bad, 2));
    int r0[] = {1, 4}, r1[] = {2, 4, 7}, r2[] = {0, 8};
    const int *runs[] = {r0, r1, r2, NULL};
    int len[] = {2, 3, 2, 0}, out[7];
    int expected[] = {0, 1, 2, 4, 4, 7, 8};
    assert(multiway_merge(runs, len, 4, out) == 7);
    for (int i = 0; i < 7; ++i)
    {
        assert(out[i] == expected[i]);
    }
    puts("07_sort: PASS (1201 comparison-sort cases, 600 radix cases, select/inversions)");
    return 0;
}
