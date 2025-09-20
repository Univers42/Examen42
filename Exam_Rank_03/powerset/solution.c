/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   powerset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 15:34:49 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 15:35:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 2

bool	is_solution(int *a, int *nums, int n, int target)
{
	int i;
	int	res;

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

void	print_solution(int *a, int *nums, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (a[i])
			printf("%d ", nums[i]);
		i++;
	}
	printf("\n");
}

void	build_candidate(int *c, int *nc)
{
	c[0] = 1;
	c[1] = 0;
	*nc = 2;
}

void	make_move(int *a, int k, int  value)
{
	a[k] = value;
}

void	unmake_move(int *a, int k, int value)
{
	(void)a;
	(void)k;
	(void)value;
}

void    powerset(int *a, int *nums, int k, int n, int target)
{
	int candidates[2];
	int nc;
	int i;

	if (k == n)
	{
		if (is_solution(a, nums, n, target))
			print_solution(a, nums, n);
		return;
	}
	else
	{
		build_candidate(candidates, &nc);
		i = -1;
		while (++i < nc)
		{
			make_move(a, k, candidates[i]);
			powerset(a, nums, k + 1, n, target);
			unmake_move(a, k, candidates[i]);
		}
	}
}

int main(int argc, char **argv)
{
	int	*a;
	int	*nums;
	int	n;
	int	target;
	int	i;
	int	k;

	if (argc == 1)
		return (1);
	n = argc - 2;
	target = atoi(argv[1]);
	a = calloc(n, sizeof(int));
	if (!a)
		return (1);
	nums = calloc(n, sizeof(int));
	if (!nums)
		return (free(a), 1);
	i = -1;
	k = 0;
	while (++i < n)
		nums[i] = atoi(argv[i + 2]);
	powerset(a, nums, k, n, target);
	return (free(a), free(nums), 0);
}