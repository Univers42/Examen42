#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>


void	print_solution(int *a, int n)
{
	int	i;
	int	first;

	i = -1;
	first = 1;
	while (++i < n)
	{
		if (!first)
			printf(" ");
		printf("%d", a[i]);
		first = 0;
	}
	printf("\n");
}

bool	is_safe(int *a, int k, int col)
{
	int	i;

	i = -1;	//i maybe has to be equal to K
	while (++i < k)
	{
		if (a[i] == col || a[i] + i == col + k || a[i] - i == col - k)
			return (false);
	}
	return (true);
}

void	build_candidate(int *candidate, int *nc, int k, int *a, int n)
{
	int	col;

	*nc = 0;
	col = -1;
	while (++col < n)
	{
		if (is_safe(a, k, col))
		{
			candidate[*nc] = col;
			(*nc)++;
		}
	}
}

void	n_queen(int *a, int k, int n)
{
	int	*candidate;
	int	nc;
	int	i;

	candidate = calloc(n, sizeof(int));
	if (!candidate)
		return ;
	if (k == n)
		print_solution(a, n);
	else
	{
		build_candidate(candidate, &nc, k, a ,n);
		i = -1;
		while (++i < nc)
		{
			a[k] = candidate[i];
			n_queen(a, k + 1, n);
		}
	}
	free(candidate);
}

int main(int argc, char **argv)
{
	int *a;
	int	n;
	int	k;

	if (argc != 2)
		return (1);
	n = atoi(argv[1]);
	a = calloc(sizeof(int), n);
	if (!a)
		return (1);
	k = 0;
	n_queen(a, k, n);
	free(a);
	return (0);
}