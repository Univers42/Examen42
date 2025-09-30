#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

static void	print_solution(int *a, int n)
{
	int	i;
	int	first = 1;

	i = -1;
	while (++i < n)
	{
		if (!first)
			printf(" ");
		printf("%d", a[i]);
		first = 0;
	}
	printf("\n");
}

//to repeat
static bool	is_safe(int *a, int k, int col)
{
	int	i;

	i = -1;
	while (++i < k) //maybe to compare with n instead
	{
		if (a[i] == col || a[i] - i == col - k || a[i] + i == col + k)
			return (false);
	}
	return  (true);
}

static void	build_candidate(int *c, int *nc, int *a, int n, int k)
{
	int	col;

	*nc = 0;
	col = -1;
	while (++col < n)
		if (is_safe(a, k, col))
			c[(*nc)++] = col;
}

void	n_queen(int *a, int k, int n)
{
	int	candidate[n];
	int	nc;
	int	i;

	if (k == n)
		print_solution(a, n);
	else
	{
		i = -1;
		build_candidate(candidate, &nc, a, n, k);
		while (++i < nc)
		{
			a[k] = candidate[i];
			n_queen(a, k + 1, n);
		}
	}
}

int	main(int argc, char **argv)
{
	int *a;
	int	n;
	int	k;

	n = atoi(argv[1]);
	k = 0;
	a = calloc(n, sizeof(int));
	n_queen(a, k, n);
	free(a);
	return(0);
}