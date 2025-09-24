#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
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
	int	idx;
	int	i;

	candidate = calloc(n, sizeof(n));
	if (!candidate)
		return ;
	if (k == n)
		puts(pattern);
	else
	{
		build_candidate(pattern, k, n, candidate, &nc);
		idx = -1;
		while (++idx < nc)
		{
			i = candidate[idx];
			ft_swap(&pattern[i], &pattern[k], sizeof(char));
			permutation(pattern, k + 1, n);
			ft_swap(&pattern[i], &pattern[k], sizeof(char));
		}
	}
	free(candidate);
}

int main(int argc, char **argv)
{
	char *pattern;
	int	k;
	int	n;

	pattern = argv[1];
	k = 0;
	n = strlen(pattern);
	permutation(pattern, k, n);
	return (0);
}