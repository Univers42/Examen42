#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool is_finished = false;

bool is_a_solution(int *a, int k, int n)
{
    /**
     * the base case is when the recursio depth
     * `k`is equal to `n`:
     * Each recursion level `k` represent placing a queen
     * in row `k`
     * when `k` == `n`, it means we have succesfully placed a queen
     * in every row without conflicts
     */
    return (k == n);
}

void print_solution(int *a, int k, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf("%c ", (a[i] == j) ? 'Q' : '.');
        printf("\n");
    }
    printf("\n");
}

/**
 * determines which candidates (choices) are valid at
 * the current step (node/branch) of the recursion.
 * we check all possible columns but only add the valid ones
 * 
 * (not attacked) to the candidate list
 * So the number of nodes per branch (candidates) can vary
 * depending on the current state
 *  */
void build_candidate(int *candidate, int *nc, int n, int k, int *a)
{
    int col, i , ok;

    *nc = 0;
    col = -1;
    while (++col < n)
    {
        ok = 1;
        i = -1;
        while (++i < k)
        {
            if (a[i] == col || a[i] - i == col - k || a[i] + i == col + k)
            {
                ok = 0;
                break;
            }
        }
        if (ok)
            candidate[(*nc)++] = col;
    }
}

static void    make_move(int *a, int k, int value)
{
    a[k] = value;
}

static void unmake_move(int *a, int k, int value)
{
    (void)a;
    (void)k;
    (void)value;
}

void backtrack(int *a, int k, int n)
{
    int candidate[n];
    int nc;
    int i;

    if (is_a_solution(a, k, n))
        print_solution(a, k, n);
    else
    {
        build_candidate(candidate, &nc, n, k, a);
        i = -1;
        while (++i < nc)
        {
            make_move(a, k, candidate[i]);
            backtrack(a, k + 1, n);
            unmake_move(a, k, candidate[i]);
        }
        if (is_finished)
            return ;
    }
}

int main(int argc, char **argv)
{
    int *a;
    int n;

    if (argc != 2)
        return (1);
    n = atoi(argv[1]);
    a = calloc(n, sizeof(int));
    backtrack(a, 0, n);
    free(a);
    return(0);
}