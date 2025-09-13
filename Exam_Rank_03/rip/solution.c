/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 20:10:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/13 20:43:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RECLVL 0
#define CANDIDATE_MAX 2

bool finished = false;

void min_removals(char *seq, int *left, int *right)
{
    *left = 0;
    *right = 0;
    for (int i = 0; seq[i]; i++)
    {
        if (seq[i] == '(')
            (*left)++;
        else if (seq[i] == ')')
        {
            if (*left > 0)
                (*left)--;
            else
                (*right)++;
        }
    }
}

bool is_balanced(char *seq, int *a, int n)
{
    int bal = 0;
    for (int i = 0; i < n; i++)
    {
        if ((seq[i] == '(' || seq[i] == ')') && a[i] == 0)
            continue;
        if (seq[i] == '(')
            bal++;
        else if (seq[i] == ')')
        {
            bal--;
            if (bal < 0)
                return false;
        }
    }
    return (bal == 0);
}

int count_removals(char *seq, int *a, int n)
{
    int rem = 0;
    for (int i = 0; i < n; i++)
        if ((seq[i] == '(' || seq[i] == ')') && a[i] == 0)
            rem++;
    return rem;
}

bool is_solution(int k, int n)
{
    return (k == n);
}

void print_solution(char *seq, int *a, int n)
{
    for (int i = 0; i < n; i++)
    {
        if ((seq[i] == '(' || seq[i] == ')') && a[i] == 0)
            write(1, "_", 1);
        else
            write(1, &seq[i], 1);
    }
    write(1, "\n", 1);
}

// Template: build_candidate
void build_candidate(char *seq, int k, int *candidate, int *nc)
{
    if (seq[k] == '(' || seq[k] == ')')
    {
        candidate[0] = 1; // keep
        candidate[1] = 0; // remove
        *nc = 2;
    }
    else
    {
        candidate[0] = 1;
        *nc = 1;
    }
}

void make_move(int *a, int k, int value)
{
    a[k] = value;
}

void unmake_move(int *a, int k, int value)
{
    (void)a;
    (void)k;
    (void)value;
}

// Backtracking
void rip(char *seq, int *a, int k, int n, int min_rem)
{
    int candidate[CANDIDATE_MAX];
    int nc;
    int next_k;
    int i;

    if (is_solution(k, n))
    {
        if (is_balanced(seq, a, n) && count_removals(seq, a, n) == min_rem)
            print_solution(seq, a, n);
        return;
    }
    else
    {
        next_k = k + 1;
        build_candidate(seq, k, candidate, &nc);
        i = -1;
        while (++i < nc)
        {
            make_move(a, k, candidate[i]);
            rip(seq, a, next_k, n, min_rem);
            unmake_move(a, k, candidate[i]);
        }
        if (finished)
            return ;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    char *seq = argv[1];
    int n = strlen(seq);
    int left, right;
    min_removals(seq, &left, &right);
    int min_rem = left + right;
    int *a = calloc(n, sizeof(int));
    rip(seq, a, RECLVL, n, min_rem);
    free(a);
    return 0;
}