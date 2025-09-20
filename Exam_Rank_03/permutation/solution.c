#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool	is_solution(int n, int k)
{
	return (n == k);
}

void	print_solution(char *pattern)
{
	puts(pattern);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const unsigned char *s = (const unsigned char*)src;
	unsigned char *d;

	d = (unsigned char*)dest;
	if (d == s)
		return (dest);
	while (n--)
		*d++ = *s++;
	return(dest);
}

void ft_swap(void *a, void *b, size_t size)
{
    unsigned char *pa = a;
    unsigned char *pb = b;
    size_t i = -1;
	
	if (a == b)
		return ;
    while (++i < size)
    	(pa[i] ^= pb[i]), (pb[i] ^= pa[i]), (pa[i] ^= pb[i]);
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
		{
			if (pattern[candidate[i]] > pattern[candidate[j]])
				ft_swap(&candidate[i], &candidate[j], sizeof(int));
		}
	}
}

void build_candidate(char *pattern, int k, int n, int *candidate, int *nc)
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

	candidate = malloc(sizeof(int) * n);
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

int main(int argc, char **argv)
{
	(void)argc;
	char *pattern = argv[1];
	int	k = 0;
	int	n = strlen(pattern);
	permutation(pattern, k, n);
	return (0);
}