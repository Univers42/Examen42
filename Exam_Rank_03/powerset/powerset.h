#ifndef POWERSET_H
# define POWERSET_H

# include <stdio.h>
# include <stdlib.h>

typedef struct s_subset
{
	int				*elements;
	int				size;
	struct s_subset	*next;
}	t_subset;

// Core functions
void	find_subsets(int *arr, int n, int target, t_subset **result);
void	backtrack(int *arr, int n, int target, int index, 
			int *current, int current_size, t_subset **result);

// Subset management
t_subset	*create_subset(int *elements, int size);
void		add_subset(t_subset **head, int *elements, int size);
void		print_and_free_subsets(t_subset *head);

// Utility functions
int		parse_arguments(int argc, char **argv, int **numbers);
void	error_exit(void);

#endif