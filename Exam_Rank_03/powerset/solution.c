#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int  cmp_int(const void *a, const void *b) {
    int ia = *(const int *)a, ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

static int  lower_bound(const int *arr, int n, int key) {
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] < key) lo = mid + 1; else hi = mid;
    }
    return lo; // index of first >= key
}

typedef struct {
    const int *orig;  // original numbers (argv[2..])
    int        n;     // count of original numbers
    const int *uniq;  // unique sorted values
    const int *cnt;   // counts per unique value
    int        m;     // number of unique values
    int       *pick;  // how many of each unique value selected
    long long  target;
} Ctx;

static void print_solution(const Ctx *ctx) {
    // Ensure non-empty subset
    int total = 0;
    for (int i = 0; i < ctx->m; ++i) total += ctx->pick[i];
    if (total == 0) return;

    // Make a mutable copy of counts picked
    int *left = (int *)malloc(sizeof(int) * ctx->m);
    if (!left) return;
    memcpy(left, ctx->pick, sizeof(int) * ctx->m);

    // Print in original input order
    int first = 1;
    for (int i = 0; i < ctx->n; ++i) {
        int v = ctx->orig[i];
        int idx = lower_bound(ctx->uniq, ctx->m, v);
        if (idx < ctx->m && ctx->uniq[idx] == v && left[idx] > 0) {
            if (!first) putchar(' ');
            printf("%d", v);
            left[idx]--;
            first = 0;
        }
    }
    putchar('\n');
    free(left);
}

static void dfs(int pos, long long sum, Ctx *ctx) {
    if (pos == ctx->m) {
        if (sum == ctx->target) print_solution(ctx);
        return;
    }
    int v = ctx->uniq[pos];
    int max_take = ctx->cnt[pos];
    for (int k = 0; k <= max_take; ++k) {
        ctx->pick[pos] = k;
        dfs(pos + 1, sum + (long long)k * v, ctx);
    }
    ctx->pick[pos] = 0;
}

int main(int argc, char **argv) {
    if (argc < 3) return 0;

    long long target = atoll(argv[1]);
    int n = argc - 2;
    int *orig = (int *)malloc(sizeof(int) * n);
    if (!orig) return 1;

    for (int i = 0; i < n; ++i) orig[i] = atoi(argv[i + 2]);

    // Build unique values with counts
    int *tmp = (int *)malloc(sizeof(int) * n);
    if (!tmp) { free(orig); return 1; }
    memcpy(tmp, orig, sizeof(int) * n);
    qsort(tmp, n, sizeof(int), cmp_int);

    int *uniq = (int *)malloc(sizeof(int) * n);
    int *cnt  = (int *)malloc(sizeof(int) * n);
    if (!uniq || !cnt) { free(orig); free(tmp); free(uniq); free(cnt); return 1; }

    int m = 0;
    for (int i = 0; i < n; ++i) {
        if (i == 0 || tmp[i] != tmp[i - 1]) {
            uniq[m] = tmp[i];
            cnt[m] = 1;
            m++;
        } else {
            cnt[m - 1]++;
        }
    }

    int *pick = (int *)calloc((size_t)m, sizeof(int));
    if (!pick) { free(orig); free(tmp); free(uniq); free(cnt); return 1; }

    Ctx ctx = {
        .orig = orig, .n = n,
        .uniq = uniq, .cnt = cnt, .m = m,
        .pick = pick, .target = target
    };

    dfs(0, 0, &ctx);

    free(pick);
    free(cnt);
    free(uniq);
    free(tmp);
    free(orig);
    return 0;
}
