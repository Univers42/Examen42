#ifndef N_QUEENS_H
# define N_QUEENS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
// Core N-Queens functions
void	solve_n_queens(int n);
void	backtrack(int *board, int col, int n, int *col_used, 
			int *diag1_used, int *diag2_used);

// Utility functions
void	print_solution(int *board, int n);
int		is_safe(int *board, int col, int row, int n);
int		abs_value(int x);

#endif