/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   combinations.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 18:11:07 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/12 18:37:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @note k is considered as the current level of recursion or the current
 * position we are  deciding.
 */
#define MAX_CANDIDATE 2
bool is_solution(char *str)
{
    return (!str);
}

void    ft_swap(char *a, char *b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void    print_solution(char *str)
{
    printf("%s\n", str);
}

void    make_move(char *str1, char *str2)
{
    ft_swap(str1, str2);
}

void unmake_move(char *str1, char *str2)
{
    ft_swap(str1, str2);
}

void    backtrack(char *str, int k, int n)
{
    int i;

    if (k == n)
        print_solution(str);
    else
    {
        i = k - 1;
        while (++i < n)
        {
            make_move(&str[k], &str[i]);
            backtrack(str, k + 1, n);
            unmake_move(&str[k], &str[i]);
        }
    }
}

int main(int argc, char **argv)
{
    char    *seq;
    int     n;

    if (argc != 2)
        return (1);
    seq = argv[1];
    n = strlen(seq);    
    backtrack(seq, 0, n);
    return (0);    
}