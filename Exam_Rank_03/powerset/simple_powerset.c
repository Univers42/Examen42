#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define MAX_CANDIDATES 2

int is_solution()
{

}

void    build_candidate(int *candidate, int *a, int *nc)
{

}

void    make_move(int *a, int k, int value)
{

}

void    unmake_move(int *a, int k, int value)
{

}

void    powerset(int target, int *nums, int *a, int k, int n)
{
    int candidate[MAX_CANDIDATES];
    int nc;

    if (k == n)
    {
        if (is_solution())
            print_solution();
    }
    else
    {
        build_candidate(candidate, &nc);
        i = -1;
        while (++i < nc)
        {
            make_move(a, k, candidate[i]);
            powerset(target, nums, a, k + 1, n);
            unmake_move(a, k, candidate[i]);
        }
    }
}

int main(int argc, char **argv)
{
    int target;
    int *nums;

    int *a;
    int n;
    int k;

    int i;

    if (argc < 3)
        exit(EXIT_FAILURE);
    n = argc - 2;
    a = calloc(n, sizeof(int));
    if (!a)
    {
        perror("Error :");
        return (2);
    }
    target = atoi(argv[1]);
    nums = calloc(n,  sizeof(int));
    if (!nums)
        return (perror("Error : "), 3);
    i = -1;
    while (++i < n)
        nums[i] = argv[i + 2];
    k = 0;
    powerset(target, nums, a, k, n);
    free(a);
    free(nums);
    return (0);
}