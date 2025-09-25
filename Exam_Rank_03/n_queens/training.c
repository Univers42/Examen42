#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 30

void	print_solution(int *a, int n)
{
	static	bool solved = false;
	int	i;
	int	first = 0;

	i = -1;
	while (++i < n)
	{
		if (first)
			fprintf(stdout, " ");
		fprintf(stdout, "%d", a[i]);
		solved = true;
		first = 1;
	}
	if (first)
		fprintf(stdout, "\n");
	if (!solved && n == 0)
		fprintf(stdout,  "\n");
}

bool	is_safe(int *a, int col, int k)
{
	int	i;

	i = -1;
	while (++i < k)
	{
		if(a[i] == col || a[i] - i == col - k|| a[i] + i == col + k)
			return (false);
	}
	return  (true);
}

void	build_candidate(int *c, int *nc, int *a, int k, int n)
{
	int	col;

	col = -1;
	*nc = 0;
	while (++col < n)
	{
		if (is_safe(a, col, k))
		{
			c[*nc] = col;
			(*nc)++;
		}
	}
}

void    n_queen(int *a, int k, int n)
{
	int	candidate[MAX_CANDIDATES];
	int	nc;
	int	i;

	if (k == n)
		print_solution(a, n);
	else
	{
		i = -1;
		build_candidate(candidate, &nc, a, k, n);
		while (++i < nc)
		{
			a[k] = candidate[i];
			n_queen(a, k + 1, n);
		}
	}
}

int main(int argc, char **argv)
{
	int k;
	int n;

	if (argc != 2)
		return (1);
	n = atoi(argv[1]);
	int	a[n];
	k = 0;
	n_queen(a, k, n);
	print_solution(a, 0);
	return (0);
}