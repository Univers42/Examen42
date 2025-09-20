/**
 * Designing an appropriate state space to represent combinatorial 
 * object starts by counting how many objects need representin. How many subsets
 * are there of an n-element
 * {1...,N}
 * if N = 1, we have {} and {1}
 * if N = 2, we have {2} {1 2} {2 1} {1} = 4
 * if N = 4, we have 8
 * so we have 2^n elements. Each new elements double the number of possibilities
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 100 // candidates for next position
bool finished = false;      // includes to allow for premature termination, which could be set
                            // in any application specific routine
/**
 * This boolean test whether the firs k element of a vector `a` form a complete solution
 * for the given problem.
 * @param input allow pass general informatin into the routine
 * we can use to specify n-te size of a targt solution
 * @note
 * This is the best case, the end case where the backtracking will give back all the frames.
 */
bool is_a_solution(int *a, int k, int n)
{
    return (k == n);
}

/**
 * This routine prints, counts, stores or processes a complete solution once it is constructed
 */
void    print_solution(int *a, int k, int n)
{
    printf("{ ");
    for (int i = 1; i <= n; i++)
        if (a[i])
            printf("%d ", i);
    printf("}\n");
}

/**
 * @brief This routine fills an array c with the complete set of possible candidates for the kth
 * position of `a`, given the contents of the first `k - 1` positions.
 * The nmber of candidates returned in this array is denoted by `nc`. Again input may be used to pass auxiliary
 * information
 */
void build_candidates(int *a, int k, int n, int *c, int *nc)
{
    (void)n;
    (void)k;
    (void)a;
    c[0] = 1;   //include this element in the subset
    c[1] = 0;   //do not incluude this element in the subset
    *nc = 2;    //tell the backatracking loop how many branches (choices) to try at this level
}

/**
 * this routine enable us to modify a data structure in response to the latest move
 * as well as clean up this data structure if we decide to take back the move.
 * Such as a data structure can always be rebuilt from scratch using the solution
 * vector `a`, but this can be innefficient when  each moves involve small incremental
 * changes that can easily be undone
 * @note when not necessar just use them as null stubs
 */
void    make_move(int *a, int k, int value)
{
    a[k] = value;
}

void    unmake_move(int *a, int k, int input)
{
    (void)a;
    (void)k;
    (void)input;
}

/**
 * print_so
 */
void backtrack(int *a, int k, int n)
{
    int c[MAX_CANDIDATES];  // Candidate for next position
    int nc;                 //next_position candidate count
    int i;                  //counter

    if (k > n)
        return ;
    if (is_a_solution(a,k, n))
        print_solution(a, k, n);
    else
    {
        k = k + 1;
        build_candidates(a, k, n, c, &nc);
        i = -1;
        while (++i < nc)
        {
            make_move(a, k, c[i]);
            backtrack(a, k, n);
            unmake_move(a, k, n);
            if (finished)
                return;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    int n = atoi(argv[1]);
    int *a = calloc(n + 1, sizeof(int));
    backtrack(a, 0, n);
    free(a);
    return (0);
}