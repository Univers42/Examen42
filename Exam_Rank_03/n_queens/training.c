#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void print_solution(int *a, int n)
{
    int i;
    int first;

    i = -1;
    first = 1;
    while (++i < n)
    {
        if (!first)
            printf(" ");
        printf("%d", a[i]);
        first = 0;
    }
    if (!first)
        printf("\n");
}

static bool is_safe(int col, int k, int *a)
{
    int i;

    i = -1;
    while (++i < k)
        if (a[i] == col || a[i] + i == col + k || a[i] - i == col - k)
            return (false);
    return (true);
}

void    build_candidate(int *c, int *nc, int *a, int k, int n)
{
    int col;

    col = -1;
    *nc = 0;
    while (++col < n)
    {
        if (is_safe(col, k, a))
        {
            c[*nc] = col;
            (*nc)++;
        }
    }
}

void n_queen(int n, int k, int *a)
{
    int candidate[n];
    int nc;
    int i;

    if (k == n)
        print_solution(a, n);
    else
    {
        i = -1;
        build_candidate(candidate, &nc, a, k, n);
        while (++i < nc)
        {
            a[k] = candidate[i];    //make move
            n_queen(n, k + 1, a);
            //unmake_move
        }
    }      
}

int main(int argc, char **argv)
{
    int n;
    int k;
    int *a;

    if (argc != 2)
        return (1);
    n = atoi(argv[1]);
    a = calloc(n, sizeof(int));
    if (!a)
        return (2);
    k = 0;
    n = atoi(argv[1]);
    n_queen(n, k, a);
    return (free(a), 0);
}