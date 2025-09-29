#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void    ft_swap(void *ptr1, void *ptr2, size_t n)
{
    unsigned char *pa;
    unsigned char *pb;

    pa = (unsigned char *)ptr1;
    pb = (unsigned char *)ptr2;
    if (pa == pb)
        return ;
    while (n--)
        (*pa ^= *pb), (*pb ^= *pa), (*pa++ ^= *pb++);
}

void    selection_sort(char *pattern, int *c, int *nc)
{
    int i;
    int j;

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

void    build_candidate(char *pattern, int *c, int *nc, int n, int k)
{
    int i;

    *nc = 0;
    i = k;
    while (i < n)
    {
        c[*nc] = i;
        (*nc)++;
        i++;
    }
    selection_sort(pattern, c, nc);
}

void    permutation(char *pattern, int k, int n)
{
    int candidate[n];
    int nc;
    int i;
    int idx;

    if (k == n)
        puts(pattern);
    else
    {
        i = -1;
        build_candidate(pattern, candidate, &nc, n, k);
        while (++i < nc)
        {
            idx = candidate[i];
            ft_swap(&pattern[idx], &pattern[k], sizeof(char));
            permutation(pattern, k + 1, n);
            ft_swap(&pattern[idx], &pattern[k], sizeof(char));
        }
    }
}

int main(int argc, char **argv)
{
    char *pattern;
    int k;
    int n;

    if (argc != 2)
        return (1);
    pattern = argv[1];
    n = strlen(pattern);
    k = 0;
    permutation(pattern, k, n);
    return (0);
}