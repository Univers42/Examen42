#include "n_queens.h"

int abs_value(int x)
{
	return (x < 0) ? -x : x;
}

void print_solution(int *board, int n)
{
	int i = 0;
	while (i < n)
	{
		fprintf(stdout, "%d", board[i]);
		if (i < n - 1)
			fprintf(stdout, " ");
		i++;
	}
	fprintf(stdout, "\n");
}

// Optimized conflict detection using arrays
void backtrack(int *board, int col, int n, int *col_used, 
		int *diag1_used, int *diag2_used)
{
	// Base case: all queens placed successfully
	if (col == n)
	{
		print_solution(board, n);
		return;
	}
	
	int row = 0;
	while (row < n)
	{
		// Check if current position is safe using lookup arrays
		// diag1: row - col + n - 1 (main diagonal)
		// diag2: row + col (anti-diagonal)
		int diag1_idx = row - col + n - 1;
		int diag2_idx = row + col;
		
		if (!col_used[row] && !diag1_used[diag1_idx] && !diag2_used[diag2_idx])
		{
			// Place queen
			board[col] = row;
			col_used[row] = 1;
			diag1_used[diag1_idx] = 1;
			diag2_used[diag2_idx] = 1;
			
			// Recurse to next column
			backtrack(board, col + 1, n, col_used, diag1_used, diag2_used);
			
			// Backtrack: remove queen
			col_used[row] = 0;
			diag1_used[diag1_idx] = 0;
			diag2_used[diag2_idx] = 0;
		}
		row++;
	}
}

void solve_n_queens(int n)
{
	if (n <= 0)
		return;
	
	// Special cases with no solutions
	if (n == 2 || n == 3)
		return;
	
	// Allocate memory for board and conflict tracking arrays
	int *board = malloc(sizeof(int) * n);
	int *col_used = malloc(sizeof(int) * n);
	int *diag1_used = malloc(sizeof(int) * (2 * n - 1));  // Main diagonals
	int *diag2_used = malloc(sizeof(int) * (2 * n - 1));  // Anti-diagonals
	
	if (!board || !col_used || !diag1_used || !diag2_used)
	{
		if (board) free(board);
		if (col_used) free(col_used);
		if (diag1_used) free(diag1_used);
		if (diag2_used) free(diag2_used);
		return;
	}
	
	// Initialize conflict tracking arrays
	int i = 0;
	while (i < n)
	{
		col_used[i] = 0;
		i++;
	}
	
	i = 0;
	while (i < 2 * n - 1)
	{
		diag1_used[i] = 0;
		diag2_used[i] = 0;
		i++;
	}
	
	// Start backtracking from column 0
	backtrack(board, 0, n, col_used, diag1_used, diag2_used);
	
	// Clean up memory
	free(board);
	free(col_used);
	free(diag1_used);
	free(diag2_used);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	
	int n = atoi(argv[1]);
	solve_n_queens(n);
	
	return 0;
}