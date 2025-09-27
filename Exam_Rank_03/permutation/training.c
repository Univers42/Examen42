#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void    ft_swap(void *a, void *b, size_t n)
{
    unsigned char *pa;
    unsigned char *pb;

    if (a == b)
        return ;
    pa = (unsigned char *)a;
    pb = (unsigned char *)b;
    while (n--)
        (*pa ^= *pb), (*pb ^= *pa), (*pa++ ^= *pb++);
}

<<<<<<< Updated upstream
void    selection_sort(char *pattern, int *c, int *nc)
{
    int i;
    int j;

=======
void    selection_sort(int *c, int *nc, char *pattern, int k)
{
    int     i;
    int     j;

    (void)k;
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
void    build_candidate(char *pattern, int *c, int *nc, int n, int k)
=======
void    build_candidate(char *pattern, int *c, int *nc, int k, int n)
>>>>>>> Stashed changes
{
    int i;

    i = k;
    *nc = 0;
<<<<<<< Updated upstream
    while(i < n)
=======
    while (i < n)
>>>>>>> Stashed changes
    {
        c[*nc] = i;
        (*nc)++;
        i++;
    }
<<<<<<< Updated upstream
    selection_sort(pattern, c, nc);
}

void    permutation(char *pattern, int *a, int k, int n)
=======
    selection_sort(c, nc, pattern, k);
}

void    permutation(char *pattern, int k, int n)
>>>>>>> Stashed changes
{
    int *candidate;
    int nc;
    int i;
    int idx;

    candidate = calloc(n, sizeof(int));
<<<<<<< Updated upstream
    if (!candidate)
        return ;
    if (k == n)
        puts(pattern);
    else
    {
        build_candidate(pattern, candidate, &nc, n, k);
        i = -1;
        while (++i < nc)
        {
            idx = candidate[i];
            ft_swap(&pattern[idx], &pattern[k],  sizeof(char));
            permutation(pattern, a, k + 1, n);
            ft_swap(&pattern[idx], &pattern[k], sizeof(char));
        }
    }
    free(candidate);
}

int main(int argc, char **argv)
{
    char    *pattern;
    int     *a;
    int     k;
    int     n;

    if (argc != 2)
        return (1);
    pattern = argv[1];
    n = strlen(pattern);
    a = calloc(n, sizeof(int));
    if (!a)
        return (1);
    pattern = argv[1];
    k = 0;
    permutation(pattern, a, k, n);
    free(a);
    return (0);
}
=======
    if (n == k)
        puts(pattern);
    else
    {
        idx = -1;
        build_candidate(pattern, candidate, &nc, k, n);
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
    int n;
    int k;

    if (argc != 2)
        return (1);
    k = 0;
    pattern = argv[1];
    n = strlen(pattern);
    permutation(pattern, k, n);
    return (0);
}
>>>>>>> Stashed changes
