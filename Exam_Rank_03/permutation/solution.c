#include <stdio.h>
#include <stdlib.h>

int	is_solution(int n, int k)
{
	return (n == k);
}

void	print_solution(char *pattern)
{
	puts(pattern);
}

void	ft_swap(void *a, void *b, size_t size)
{
	unsigned char	*pa;
	unsigned char	*pb;
	size_t			i;

	if (a == b)
		return ;
	pa = (unsigned char *)a;
	pb = (unsigned char *)b;
	i = 0;
	while (i < size)
	{
		pa[i] ^= pb[i];
		pb[i] ^= pa[i];
		pa[i] ^= pb[i];
		i++;
	}
}

void	make_move(char *a, char *b)
{
	ft_swap(a, b, sizeof(char));
}

void	unmake_move(char *a, char *b)
{
	ft_swap(a, b, sizeof(char));
}

void	selection_sort(char *pattern, int *candidate, int *nc)
{
	int	i;
	int	j;

	i = -1;
	while (++i < *nc - 1)
	{
		j = i;
		while (++j < *nc)
			if (pattern[candidate[i]] > pattern[candidate[j]])
				ft_swap(&candidate[i], &candidate[j], sizeof(int));
	}
}

void	build_candidate(char *pattern, int k, int n, int *candidate, int *nc)
{
	int	i;

	*nc = 0;
	i = k;
	while (i < n)
	{
		candidate[*nc] = i;
		(*nc)++;
		i++;
	}
	selection_sort(pattern, candidate, nc);
}

void	permutation(char *pattern, int k, int n)
{
	int	*candidate;
	int	nc;
	int	i;
	int	idx;

	candidate = (int *)malloc(sizeof(int) * n);
	if (!candidate)
		return ;
	if (is_solution(k, n))
		print_solution(pattern);
	else
	{
		build_candidate(pattern, k, n, candidate, &nc);
		idx = -1;
		while (++idx < nc)
		{
			i = candidate[idx];
			make_move(&pattern[i], &pattern[k]);
			permutation(pattern, k + 1, n);
			unmake_move(&pattern[i], &pattern[k]);
		}
	}
	free(candidate);
}

static int	ft_strlen(const char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return (i);
}

int	main(int argc, char **argv)
{
	char	*src;
	char	*pattern;
	int		k;
	int		n;
	int		i;

	if (argc < 2 || !argv[1] || !argv[1][0])
		return (0);
	src = argv[1];
	n = ft_strlen(src);
	pattern = (char *)malloc((size_t)n + 1);
	if (!pattern)
		return (1);
	i = 0;
	while (i < n)
	{
		pattern[i] = src[i];
		i++;
	}
	pattern[n] = '\0';
	k = 0;
	permutation(pattern, k, n);
	free(pattern);
	return (0);
}