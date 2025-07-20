#include "rip.h"

// Utility functions
int str_len(char *str)
{
	int len = 0;
	while (str[len])
		len++;
	return len;
}

void str_copy(char *dest, char *src, int len)
{
	int i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

int str_equal(char *s1, char *s2, int len)
{
	int i = 0;
	while (i < len)
	{
		if (s1[i] != s2[i])
			return 0;
		i++;
	}
	return 1;
}

// Count excess opening and closing parentheses
void count_invalid_parens(char *input, int *excess_open, int *excess_close)
{
	int open = 0;
	int close = 0;
	int i = 0;
	
	// First pass: count unmatched closing parens
	while (input[i])
	{
		if (input[i] == '(')
			open++;
		else if (input[i] == ')' && open > 0)
			open--;
		else if (input[i] == ')')
			close++;
		i++;
	}
	
	*excess_open = open;   // Unmatched opening parens
	*excess_close = close; // Unmatched closing parens
}

// Solution management
t_solution *create_solution(char *str, int len)
{
	t_solution *sol = malloc(sizeof(t_solution));
	if (!sol)
		return NULL;
	
	sol->result = malloc(len + 1);
	if (!sol->result)
	{
		free(sol);
		return NULL;
	}
	
	str_copy(sol->result, str, len);
	sol->next = NULL;
	return sol;
}

int is_duplicate(t_solution *head, char *str, int len)
{
	while (head)
	{
		if (str_equal(head->result, str, len) && head->result[len] == '\0')
			return 1;
		head = head->next;
	}
	return 0;
}

void add_solution(t_solution **head, char *str, int len)
{
	if (is_duplicate(*head, str, len))
		return;
	
	t_solution *new_sol = create_solution(str, len);
	if (!new_sol)
		return;
	
	new_sol->next = *head;
	*head = new_sol;
}

void print_and_free_solutions(t_solution *head)
{
	t_solution *temp;
	
	while (head)
	{
		puts(head->result);
		temp = head;
		head = head->next;
		free(temp->result);
		free(temp);
	}
}

// Core backtracking algorithm
void generate_solutions(char *input, int len, int pos, char *current,
		int open_count, int to_remove_open, int to_remove_close,
		t_solution **solutions)
{
	// Base case: processed all characters
	if (pos == len)
	{
		if (to_remove_open == 0 && to_remove_close == 0 && open_count == 0)
			add_solution(solutions, current, len);
		return;
	}
	
	char ch = input[pos];
	
	if (ch == '(')
	{
		// Option 1: Remove this opening paren (if we still need to remove some)
		if (to_remove_open > 0)
		{
			current[pos] = ' ';
			generate_solutions(input, len, pos + 1, current,
				open_count, to_remove_open - 1, to_remove_close, solutions);
		}
		
		// Option 2: Keep this opening paren
		current[pos] = '(';
		generate_solutions(input, len, pos + 1, current,
			open_count + 1, to_remove_open, to_remove_close, solutions);
	}
	else if (ch == ')')
	{
		// Option 1: Remove this closing paren (if we still need to remove some)
		if (to_remove_close > 0)
		{
			current[pos] = ' ';
			generate_solutions(input, len, pos + 1, current,
				open_count, to_remove_open, to_remove_close - 1, solutions);
		}
		
		// Option 2: Keep this closing paren (only if we have unmatched opening parens)
		if (open_count > 0)
		{
			current[pos] = ')';
			generate_solutions(input, len, pos + 1, current,
				open_count - 1, to_remove_open, to_remove_close, solutions);
		}
	}
	else
	{
		// Keep other characters as is
		current[pos] = ch;
		generate_solutions(input, len, pos + 1, current,
			open_count, to_remove_open, to_remove_close, solutions);
	}
}

void find_all_solutions(char *input)
{
	int len = str_len(input);
	int excess_open, excess_close;
	
	// Count how many parens we need to remove
	count_invalid_parens(input, &excess_open, &excess_close);
	
	// Allocate working buffer
	char *current = malloc(len + 1);
	if (!current)
		return;
	
	t_solution *solutions = NULL;
	
	// Generate all valid solutions
	generate_solutions(input, len, 0, current, 0, excess_open, excess_close, &solutions);
	
	// Print and cleanup
	print_and_free_solutions(solutions);
	free(current);
}

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1])
		return 1;
	
	find_all_solutions(argv[1]);
	return 0;
}