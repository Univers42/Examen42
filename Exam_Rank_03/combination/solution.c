/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 21:24:02 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 21:42:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool	is_solution(int k, int n)
{
	return (k == n);
}

void	print_solution(int *a, int k, int n)
{
	int	i;
	int	has_elem = 0;

	(void)k;
	i = -1;
	printf("{ ");
	while (++i < n)
	{
		if (a[i])
		{
			printf("%d ", i);
			has_elem = 1;
		}	
	}
	if (has_elem)
		printf("}\n");
}

void	build_candidate(int *c, int *nc)
{
	*nc = 2;
	c[0] = 1;
	c[1] = 0;
}

void	make_move(int *a, int k, int value)
{
	a[k] = value;
}

void	unmake_move(int *a, int k, int value)
{
	(void)a;
	(void)k;
	(void)value;
}

void    combination(int *a, int k, int n)
{
	int	*candidates;
	int	nc;
	int	i;

	candidates = malloc(sizeof(int) * n);
	if (is_solution(k, n))
		return print_solution(a, k, n);
	else
	{
		build_candidate(candidates, &nc);
		i = -1;
		while (++i < nc)
		{
			make_move(a, k, candidates[i]);
			combination(a, k + 1, n);
			unmake_move(a, k, candidates[i]);
		}
	}
	free(candidates);
}

int main(int argc, char **argv)
{
	int *a;
	int k;
	int n;

	k = 0;
	if (argc != 2)
		return (1);
	n = atoi(argv[1]);
	a = malloc(sizeof(int) * n);
	combination(a, k, n);
	free(a);
	return (0);
}