#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int     ft_strlen(const char *str)
{
    int i;

    i = 0;
    while (str[i])
        i++;
    return (i);
}

void    ft_swap(void *a, void *b, size_t n)
{
    unsigned char *pa;
    unsigned char *pb;

    pa = (unsigned char *)a;
    pb = (unsigned char *)b;

    if (*pa == *pb)
        return ;
    while (n--)
        (*pa ^= *pb),(*pb ^= *pa),(*pa++ ^= *pb++);
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

void    build_candidate(int *c, int *nc, int n, char *pattern, int k)
{
    int i;

    i = k;
    *nc = 0;
    while (i < n)
    {
        c[*nc] = i;
        (*nc)++;
        i++;
    }
    selection_sort(pattern, c, nc);
}

void    make_move(char *a, char *b)
{
    ft_swap(a, b, sizeof(char));
}

void unmake_move(char *a, char *b)
{
    ft_swap(a, b, sizeof(char));
}

void    permutation(char *pattern, int k, int n)
{
    int *candidate;
    int nc;
    int i;
    int idx;

    candidate = (int *)calloc(n, sizeof(int));
    if (n == k)
        puts(pattern);
    else
    {
        build_candidate(candidate, &nc, n, pattern, k);
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
    char    *pattern;
    int     n;
    int     k;

    if (argc != 2)
        return (1);
    pattern = argv[1];
    n = ft_strlen(pattern);
    k = 0;
    permutation (pattern, k, n);
    return (0);
}
