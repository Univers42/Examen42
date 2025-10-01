#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool	is_solution(int target, int *a, int n, int *nums)
{
	int	res;
	int	i;

	res = 0;
	i = -1;
	while (++i < n)
		if (a[i])
			res += nums[i];
	return (target == res);
}

void	print_solution(int *a, int *nums, int n)
{
	int	first = 1;
	int	i = -1;

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
	printf("\n");
}

void	build_candidate(int *c, int *nc)
{
	*nc = 2;
	c[0] = 1;
	c[1] = 0;
}

void	powerset(int target, int *nums, int  *a, int k, int n)
{
	int	candidate[2];
	int	nc;
	int	i;

	if (k == n)
	{
		if (is_solution(target, a, n, nums))
			print_solution(a, nums, n);	
	}
	else
	{
		i = -1;
		build_candidate(candidate, &nc);
		while (++i < nc)
		{
			a[k] = candidate[i];
			powerset(target, nums, a, k + 1, n);
		}
	}
}

int main(int argc, char **argv)
{
	int target;
	int *nums;
	int *a;
	int k;
	int n;
	int	i;

	if (argc < 3)
		return (1);
	target = atoi(argv[1]);
	n = argc - 2;
	a = calloc(n, sizeof(int));
	if (!a)
		return  (1);
	nums = calloc(n, sizeof(int));
	if (!nums)
		return  (free(a), 2);
	i = -1;
	while (++i < n)
		nums[i] = atoi(argv[i + 2]);
	k = 0;
	powerset(target, nums, a, k, n);
	return (free(nums),free(a), 0);
}