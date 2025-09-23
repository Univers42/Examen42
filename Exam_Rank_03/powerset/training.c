/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   training.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 14:31:34 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/23 17:42:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 2

bool    is_solution(int target, int *nums, int *a, int n)
{
    int i;
    int res;

    i = 0;
    res = 0;
    while (i < n)
    {
        if (a[i])
            res += nums[i];
        i++;
    }
    if (res == target)
        return (true);
    return (false);
}

void    print_solution(int *nums,  int *a, int n)
{
    int i;
    int first = 1;

    i = -1;
    while (++i < n)
    {
        if (a[i])
        {
            if (!first)
                printf(" ");
            printf("%d", nums[i]);
            first = 0;
        }
    }
    if (!first)
        printf("\n");
}

void build_candidate(int *c, int *nc)
{
    c[0] = 1;
    c[1] = 0;
    *nc = 2;
}

void    make_move(int *a, int k, int value)
{
    a[k] =  value;
}

void    unmake_move(int *a, int k, int value)
{
    (void)a;
    (void)k;
    (void)value;
}

void    powerset(int target, int *nums, int *a, int k, int n)
{
    int candidate[MAX_CANDIDATES];
    int nc;
    int i;

    if (k == n)
    {
        if (is_solution(target, nums, a, n))
            print_solution(nums,a,n);
    }
    else
    {
        i = -1;
        build_candidate(candidate, &nc);
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
    int *nums;
    int *a;
    int target;
    int i;
    int k;
    int n;

    if (argc < 2)
        return (1);
    n = argc - 2;
    target = atoi(argv[1]);
    a = calloc(n, sizeof(int));
    if (!a)
            return (2);
    nums = calloc(n, sizeof(int));
    if (!nums)
        return (free(a), a = NULL, 3);
    i = -1;
    while (++i < n)
        nums[i] = atoi(argv[i + 2]);
    k = 0;
    powerset(target, nums, a, k, n);
    free(a);
    free(nums);
    return (0);
}
