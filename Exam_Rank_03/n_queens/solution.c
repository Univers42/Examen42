#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Add fast bitmask state for columns and diagonals
static int g_n;
// use 64-bit for columns (n <= 64 fits)
static unsigned long long g_cols_mask;
// use 128-bit for diagonals so we can handle up to 2*n-1 bits when n <= 64
static unsigned __int128 g_diag1_mask, g_diag2_mask;

bool		is_solution(int n, int k)
{
	return(k == n);
}

void print_solution(int *a, int n)
{
    int i;

    i = -1;
    while (++i < n)
    {
        fprintf(stdout, "%d", a[i]);
        if (i < n - 1)
            fprintf(stdout, " ");
    }
    fprintf(stdout, "\n");
}

bool is_safe(int *a, int k, int candidate)
{
	// Fast path with bitmasks (safe up to n <= 64)
	if (g_n <= 64)
	{
		unsigned int d1 = (unsigned int)(k - candidate + (g_n - 1));
		unsigned int d2 = (unsigned int)(k + candidate);
		if ((g_cols_mask >> candidate) & 1ULL) return false;
		if (g_diag1_mask & (((unsigned __int128)1) << d1)) return false;
		if (g_diag2_mask & (((unsigned __int128)1) << d2)) return false;
		return true;
	}
	// Fallback: original O(k) checks for larger n
	int	i = -1;
	while (++i < k)
	{
		if (a[i] == candidate || a[i] - i == candidate - k || a[i] + i == candidate + k)
			return (false);
	}
	return (true);
}

//fills candidate[] with valid columns for row k, returns number of candidates in *nc
void build_candidate(int *a, int k, int n, int *candidate, int *nc)
{
    int col;

    *nc = 0;
    col = -1;
    while (++col < n)
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
	// Track occupancy in bitmasks for fast checks
	if (g_n <= 64)
	{
		unsigned int d1 = (unsigned int)(k - value + (g_n - 1));
		unsigned int d2 = (unsigned int)(k + value);
		g_cols_mask |= (1ULL << value);
		g_diag1_mask |= ((unsigned __int128)1) << d1;
		g_diag2_mask |= ((unsigned __int128)1) << d2;
	}
}

// New: undo move to keep bitmasks consistent
void	unmake_move(int *a, int k)
{
	if (g_n <= 64)
	{
		int value = a[k];
		unsigned int d1 = (unsigned int)(k - value + (g_n - 1));
		unsigned int d2 = (unsigned int)(k + value);
		g_cols_mask &= ~(1ULL << value);
		g_diag1_mask &= ~(((unsigned __int128)1) << d1);
		g_diag2_mask &= ~(((unsigned __int128)1) << d2);
	}
}

void n_queen(int *a, int k, int n)
{
    int	candidate[n];
    int nc;
    int i;

    if (is_solution(n, k))
        print_solution(a, n);
    else
    {
        build_candidate(a, k, n, candidate, &nc);
        i = -1;
        while (++i < nc)
        {
            make_move(a, k, candidate[i]);
            n_queen(a, k + 1, n);
            unmake_move(a, k);
        }
    }
}

int main(int argc, char **argv)
{
	// Fix: check argc before accessing argv[1]
	if (argc != 2)
		return 1;
	int n = atoi(argv[1]);
	if (n <= 0)
		return 1;

	g_n = n;
	g_cols_mask = 0ULL;
	g_diag1_mask = 0;
	g_diag2_mask = 0;

	int a[n];
	n_queen(a, 0, n);
	return 0;
}