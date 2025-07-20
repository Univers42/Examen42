#ifndef RIP_H
# define RIP_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_solution
{
	char				*result;
	struct s_solution	*next;
}	t_solution;

// Core functions
void	find_all_solutions(char *input);
void	generate_solutions(char *input, int len, int pos, char *current,
			int open_count, int to_remove_open, int to_remove_close,
			t_solution **solutions);

// Solution management
t_solution	*create_solution(char *str, int len);
void		add_solution(t_solution **head, char *str, int len);
int			is_duplicate(t_solution *head, char *str, int len);
void		print_and_free_solutions(t_solution *head);

// Utility functions
int		str_len(char *str);
void	str_copy(char *dest, char *src, int len);
int		str_equal(char *s1, char *s2, int len);
void	count_invalid_parens(char *input, int *excess_open, int *excess_close);

#endif