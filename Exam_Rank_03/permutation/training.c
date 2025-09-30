#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

int	ft_strlen(const char *str)
{
	const char *tmp;

	tmp = str;
	while (*tmp++)
		;
	return (tmp - str - 1);
}

void	ft_swap(void *a, void *b, size_t n)
{
	unsigned char *pa = (unsigned char *)a;
	unsigned char *pb = (unsigned char *)b;

	if (pa == pb)
		return ;
	while (n--)
		(*pa ^= *pb), (*pb ^= *pa), (*pa++ ^= *pb++);
}

void	selection_sort(int *c, int *nc, char *pattern)
{
	int	i;
	int	j;

	i = -1;
	while (++i < *nc - 1)
	{
		j = i;
		while (++j < *nc)
		{
			if (pattern[c[i]] > pattern[c[j]])
				ft_swap(&c[i], &c[j], sizeof(int));
		}
	}
}

void	build_candidate(int *c, int *nc, char *pattern, int k, int n)
{
	int	i;

	i = k;
	*nc = 0;
	while (i < n)
		c[(*nc)++] = i++;
	selection_sort(c, nc, pattern);
}

void	permutation(char *pattern, int k, int n)
{
	int	candidate[n];		//the error was here
	int	nc;
	int	i;
	int	idx;

	if (k == n)
		puts(pattern);
	else
	{
		i = -1;
		build_candidate(candidate, &nc, pattern, k, n);
		while (++i < nc)
		{
			idx = candidate[i];
			ft_swap(&pattern[idx], &pattern[k], sizeof(char));
			permutation(pattern, k + 1, n);
			ft_swap(&pattern[idx], &pattern[k], sizeof(char));			
		}
	}
}

int	main(int argc, char **argv)
{
	char	*pattern;
	int		n;
	int		k;

	if (argc != 2)
		return (1);
	pattern = argv[1];
	n = ft_strlen(pattern);
	k = 0;
	permutation(pattern, k, n);
	return (0);
}