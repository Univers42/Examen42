#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 2

bool	is_solution(int target, int *nums, int *a, int n)
{
	int	i;
	int	res;

	res = 0;
	i = -1;
	while (++i < n)
		if (a[i])
			res += nums[i];
	return (res == target);
}

void    print_solution(int *nums, int *a, int n)
{
	int	i;
	int	first;

	i = -1;
	first = 1;
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

void    powerset(int target, int *nums, int *a, int k, int n)
{
	int	candidate[MAX_CANDIDATES];
	int	nc;
	int	i;

	if (k == n)
	{
		if (is_solution(target, nums, a, n))
			print_solution(nums, a, n);
	}
	else
	{
		i = -1;
		candidate[0] = 1;
		candidate[1] = 0;
		nc = 2;
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
	int i;

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
	k = 0;
	i = -1;
	while (++i < n)
		nums[i] = atoi(argv[i + 2]);
	powerset(target, nums, a, k, n);
	return (free(a), free(nums), 0);
	
}