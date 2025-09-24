#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

# define MAX_CANDIDATES 2

bool    is_solution(int target, int *nums, int *a, int n)
{
    int i;
    int res;

    i = -1;
    res = 0;
    while (++i < n)
    {
        if (a[i])
            res += nums[i];
    }
    if (res == target)
        return (true);
    return (false);
}

void    print_solution(int *nums, int *a, int n)
{
    static bool any_solution = false;
    int i;
    bool found = false;

    for (i = 0; i < n; i++)
    {
        if (a[i])
        {
            if (found)
                printf(" ");
            printf("%d", nums[i]);
            found = true;
        }
    }
    if (found && n != 0)
    {
        printf("\n");
        any_solution = true;
    }
    // Only print a newline at the end if no solution was printed
    if (n == 0 && !any_solution)
        printf("\n");
}

void    build_candidate(int *c, int *nc)
{
    c[0] = 1;
    c[1] = 0;
    *nc = 2;
}

void    powerset(int target, int *nums, int *a, int k, int n)
{
    int candidate[MAX_CANDIDATES];
    int nc;
    int i;

    if (k == n)
    {
        if (is_solution(target, nums, a, n))
            print_solution(nums, a, n);
    }
    else
    {
        i = -1;
        build_candidate(candidate, &nc);
        while (++i  < nc)
        {
            a[k] = candidate[i];
            powerset(target, nums, a, k + 1, n);
        }
    }
}

int main(int argc, const char **argv)
{
    int target;
    int *nums;
    int n;
    int *a;
    int i;
    int k;

    if (argc < 3)
        return (1);
    target = atoi(argv[1]);
    n = argc - 2;
    a = calloc(n, sizeof(int));
    if (!a)
        return (2);
    nums = calloc(n, sizeof(int));
    if (!nums)
        return (free(a), 3);
    i = -1;
    while (++i < n)
        nums[i] = atoi(argv[i + 2]);
    k = 0;
    powerset(target, nums, a, k, n);
    print_solution(nums, a, 0);
    return (free(a), free(nums), 0);
}
