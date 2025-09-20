#ifndef PERMUTATIONS_H
# define PERMUTATIONS_H

# include <stdlib.h>
# include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct s_perm_list
{
	char				*perm;
	struct s_perm_list	*next;
}	t_perm_list;

// Core permutation functions
void		generate_permutations(char *str, int len, t_perm_list **list);
void		heap_permute(char *str, int size, int n, t_perm_list **list);

// List management
t_perm_list	*create_node(char *str, int len);
void		add_to_list(t_perm_list **list, char *str, int len);
void		sort_list(t_perm_list **list);
void		print_and_free_list(t_perm_list *list);

// Utility functions
void		swap_chars(char *a, char *b);
int			str_compare(const char *s1, const char *s2);
int			str_len(const char *str);
void		str_copy(char *dest, const char *src, int len);

#endif