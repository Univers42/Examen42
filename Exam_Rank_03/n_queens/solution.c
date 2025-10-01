#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_NUMBERS 100

static void	print_solution(int *a, int n)
{
	int	i;
	int	first = 1;

	i = -1;
	while (++i < n)
	{
		if (!first)
			fprintf(stdout, " ");
		fprintf(stdout, "%d", a[i]);
		first = 0;
	}
	fprintf(stdout, "\n");
}

bool	is_safe(int  *a,  int k, int col)
{
	int	i;

	i = -1;
	while (++i < k)
		if (a[i] == col || a[i] + i == col + k || a[i] - i == col - k)
			return (false);
	return (true);
}

void	build_candidate(int *c, int  *nc, int *a, int n, int k)
{
	int	col;

	*nc = 0;
	col = -1;
	while (++col < n)
		if (is_safe(a, k, col))
			c[(*nc)++] = col;
}

void    n_queen(int *a, int k, int n)
{
	int	candidate[n];
	int	nc;
	int	i;

	if (k == n)
	{
		print_solution(a, n);
	}
	else
	{
		build_candidate(candidate, &nc, a, n, k);
		i = -1;
		while (++i < nc)
		{
			a[k] = candidate[i];
			n_queen(a, k + 1, n);
		}
	}
}

int main(int argc, char **argv)
{
	int a[MAX_NUMBERS];
	int k;
	int n;

	if (argc != 2)
		return (1);
	k = 0;
	n = atoi(argv[1]);
	n_queen(a, k, n);
	return (0);
}
