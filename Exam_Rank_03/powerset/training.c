#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 2

bool is_solution(int target, int *nums, int *a, int n)
{
	int i;
	int	res;

	i = -1;
	res = 0;
	while (++i < n)
	{
		if (a[i])
			res += nums[i];
	}
	if (res == target)
		return (true);
	return (false);
}

void	print_solution(int *nums, int *a, int n)
{
	static bool solved = false;
	bool found = false;
	int	i;

	i = -1;
	while (++i < n)
	{
		if (a[i])
		{
			if (found)
				printf(" ");
			printf("%d", nums[i]);
			solved = true;
			found = true;
		}
	}
	if (found && n > 0)
		printf("\n");
	if (!solved && n == 0)
		printf("\n");
}

void	build_candidate(int *c, int *nc)
{
	*nc = 2;
	c[0] = 1;
	c[1] = 0;
}

void    powerset(int target, int *nums, int *a, int k, int n)
{
	int candidate[MAX_CANDIDATES];
	int nc;
	int i;

	if (k == n)
	{
		if (is_solution(target, nums, a, n))
			print_solution(nums, a, n);
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
	//general variables
	int *a;
	int k;
	int n;
	int i;
	//ctx_variables
	int target;
	int *nums;

	if (argc <= 2)
		return (1);
	n = argc - 2;
	a = calloc(n, sizeof(int));
	if (!a)
		return (1);
	k = 0;
	target = atoi(argv[1]);
	nums = calloc(n, sizeof(int));
	if (!nums)
		return (free(a), 1);
	i = -1;
	while (++i < n)
		nums[i] = atoi(argv[i + 2]);
	powerset(target, nums, a, k, n);
	print_solution(nums, a, 0);
	free(a);
	free(nums);
	return (0);
}