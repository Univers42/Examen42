#include "powerset.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

# define RECLVL 0
# define MAX_CANDIDATE 2
/**
 * why use k as the limit ?
 * at each recursion level, we are building a partial solution of length `k`
 * The array `a` holds  the current subset ( or choices) up to depth `k`
 * so when checking if we have a solution, we want ot sum the element that have been decided so far
 * if we have 5 numbers but are at the recursion depth `k = 3`
 * only the first 3 choices in `a`are meaningfulñ at this recursion depth
 */
bool is_a_solution(int *a, int *arr, int k, int target)
{
    int sum = 0;
    for (int i = 0; i < k; i++)
        if (a[i])
            sum += arr[i];
    return (sum == target);
}


void print_solution(int *a, int *arr, int k)
{
	int	i;

	i = -1;
    printf("{ ");
    while (++i < k)
        if (a[i])
            printf("%d ", arr[i]);
    printf("}\n");
}


void build_candidate(int *c, int *nc)
{
	c[0] = 1; //include
	c[1] = 0; //exclude
	*nc = 2;
}

void make_move(int *a, int k, int value)
{
	a[k] = value;
}

void	unmake_move(int *a, int k, int value)
{
	(void)a,
	(void)k;
	(void)value;
}

void powerset(int *a, int *arr, int k, int n, int target)
{
    int candidate[MAX_CANDIDATE];
    int nc;
	int	i;
    int new_k;

    if (k == n)
    {
        if (is_a_solution(a, arr, k, target))
            print_solution(a, arr, k);
    }
    else
    {
        build_candidate(candidate, &nc);
        new_k = k + 1;
        i = -1;
        while (++i < nc)
        {
            make_move(a, k, candidate[i]);
            powerset(a, arr, new_k , n, target);
            unmake_move(a, k, candidate[i]);
        }
    }
}

int main(int argc, char **argv)
{
    int *arr;
    int *a;
    int target;
    int n;
	int	i;

    if (argc < 3)
        return (1);
    target = atoi(argv[1]);
    n = argc - 2;
    arr = calloc(n, sizeof(int));
    a = calloc(n, sizeof(int));
	i = -1;
    while (++i < n)
        arr[i] = atoi(argv[i + 2]);
    powerset(a, arr, RECLVL, n, target);
    free(arr);
    free(a);
    return (0);
}