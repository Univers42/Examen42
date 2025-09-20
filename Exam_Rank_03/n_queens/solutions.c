#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool		is_solution(int n, int k)
{
	return(k == n);
}

void print_solution(int *a, int n)
{
    for (int i = 0; i < n; i++) {
        fprintf(stdout, "%d", a[i]);
        if (i < n - 1)
            fprintf(stdout, " ");
    }
    fprintf(stdout, "\n");
}

bool is_safe(int *a, int k, int candidate)
{
	int	i;

	i = -1;
	while (++i < k)
	{
		if (a[i] == candidate || a[i] -i == candidate - k || a[i] + i == candidate + k)
			return (false);
	}
	return (true);
}

//fills candidate[] with valid columns for row k, returns number of candidates in *nc
void build_candidate(int *a, int k, int n, int *candidate, int *nc)
{
    *nc = 0;
    for (int col = 0; col < n; col++)
    {
        if (is_safe(a, k, col))
        {
            candidate[*nc] = col;
            (*nc)++;
        }
    }
}

void	make_move(int *a, int k, int value)
{
	a[k] = value;
}


void n_queen(int *a, int k, int n)
{
    int	*candidate;
    int nc;
    int i;

	candidate = malloc(sizeof(int) * n);
	if (!candidate)
		return ;
    if (is_solution(n, k))
        print_solution(a, n);
    else
    {
        build_candidate(a, k, n, candidate, &nc);
        i = 0;
        while (i < nc)
        {
            make_move(a, k, candidate[i]);
            n_queen(a, k + 1, n);
            i++;
        }
    }
	free(candidate);
}

int main(int argc, char **argv)
{
    int n;
    int *a;

    if (argc != 2)
        return 1;
    n = atoi(argv[1]);
    if (n <= 0)
        return 1;
    a = malloc(sizeof(int) * n);
    if (!a)
        return 1;
    n_queen(a, 0, n);
    free(a);
    return 0;
}