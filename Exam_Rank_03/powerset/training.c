#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_ctx
{
    const int   *orig;      // original numbers (argv[2..])
    int         n;          //count of original numbers
    const int   *uniq;       //unique sorted values
    const int   *cnt;       // counts per unique values
    int         m;          // number of unique values
    int         *pick;      //how many of each unique value
    long long   target;     
}t_ctx;

static int cmp_int(const void *a, const void *b)
{
    int ia;
    int ib;

    ia = *(const int *)a;
    ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

static int  lower_bound(const int *arr, int n, int key)
{
    int lo;
    int hi;
    int mid;

    lo = 0;
    hi = n;
    while (lo < hi)
    {
        mid = lo + (hi - lo) / 2;
        if (arr[mid] < key)
            lo = mid + 1;
        else
            hi = mid;
    }
    return (lo);
}

static void print_solution(const t_ctx *ctx)
{
    int total;
    int i;
    int  *left;
    int first;
    int v;
    int idx;

    total = 0;
    i = 0;
    while (i < ctx->m)
        total += ctx->pick[++i];
    left = (int *)calloc(ctx->m, sizeof(int));
    if (!left)
        return ;
    memcpy(left, ctx->pick, sizeof(int) * ctx->m);
    first = 1;
    i = 0;
    while (++i < ctx->n)
    {
        v = ctx->origi[i];
        idx = lower_bound(ctx->uniq, ctx->m, v);
        if (idx < ctx->m && ctx->uniq[idx] == v && left[idx] > 0)
        {
            if (!first)
                putchar(' ');
            printf("%d", v);
            left[idx]--;
            first = 0;
        }
    }
    putchar('\n');
    free(left);
}

static void dfs(int pos, long long sum, t_ctx *ctx)
{
    int v;
    int max_take;
    int k;

    if (pos == ctx->m)
        if (sum == ctx->target)
            print_solution(ctx);
    else
    {
        v = ctx->uniq[pos];
        max_take = ctx->cnt[pos];
        k = 0;
        while (k <= max_take)
        {
            ctx->pick[pos] = k;
            dfs(pos + 1, sum + (long long)k * v, ctx);
        }
        ctx->pick[pos] = 0;
    }
}

int main(int argc, char **argv)
{
    long long   target;
    int         n;
    int         *orig;

    target = atoll(argv[1]);
    n = argc - 2;
    orig = (int *)malloc(sizeof(int) * n);
    if (!orig) return 1;
    i = 0;
    while (++i < n)
        orig[i] = atoi(argv[i + 2]);
    if (argc < 3)
        return (0);


}