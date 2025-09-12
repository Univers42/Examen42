/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   all_subsets.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 16:38:05 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/12 18:10:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

# define MAX_CANDIDATE 2

bool is_finished = false;

bool is_complete(int k, int n)
{
    return (k == n);
}

void    print_solution(int *a, int n)
{
    int i = 0;
    printf("{ ");
    while (++i <= n)
        if (a[i])
            printf("%d ", i);
    printf("}\n");   
}

void    build_candidate(int *c, int *nc)
{
    c[0] = 1;
    c[1] = 0;
    *nc = MAX_CANDIDATE;
}

void    make_move(int *a, int k, int value)
{
    a[k] = value;
}

void    unmake_move(int *a, int k, int n)
{
    (void)a;
    (void)k;
    (void)n;    
}

void    backtrack(int *a, int k, int n)
{
    int candidate[MAX_CANDIDATE];
    int nc;
    int i;

    if (is_complete(k, n))
        print_solution(a, n);
    else
    {
        k = k + 1;
        build_candidate(candidate, &nc);
        i = -1;        
        while (++i < nc)
        {
            make_move(a, k, candidate[i]);
            backtrack(a, k, n);
            unmake_move(a, k, n);
        }
        if (is_finished)
            return ;
    }
}

int main(int argc, char **argv)
{
    int n;
    int *a;

    if (argc != 2)
        return (1);
    n = atoi(argv[1]);
    a = calloc(n + 1, sizeof(int));   
    backtrack(a, 0, n);
    free(a);
    return (0);
}