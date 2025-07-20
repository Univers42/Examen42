#include "permutations.h"

// Utility functions
int str_len(const char *str)
{
	int len = 0;
	while (str[len])
		len++;
	return len;
}

void str_copy(char *dest, const char *src, int len)
{
	int i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

int str_compare(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

void swap_chars(char *a, char *b)
{
	char temp = *a;
	*a = *b;
	*b = temp;
}

// List management functions
t_perm_list *create_node(char *str, int len)
{
	t_perm_list *node = malloc(sizeof(t_perm_list));
	if (!node)
		return NULL;
	
	node->perm = malloc(len + 1);
	if (!node->perm)
	{
		free(node);
		return NULL;
	}
	
	str_copy(node->perm, str, len);
	node->next = NULL;
	return node;
}

void add_to_list(t_perm_list **list, char *str, int len)
{
	t_perm_list *new_node = create_node(str, len);
	if (!new_node)
		return;
	
	new_node->next = *list;
	*list = new_node;
}

void sort_list(t_perm_list **list)
{
	if (!*list || !(*list)->next)
		return;
	
	t_perm_list *current;
	t_perm_list *next_node;
	char *temp;
	int swapped = 1;
	
	while (swapped)
	{
		swapped = 0;
		current = *list;
		
		while (current->next)
		{
			next_node = current->next;
			if (str_compare(current->perm, next_node->perm) > 0)
			{
				temp = current->perm;
				current->perm = next_node->perm;
				next_node->perm = temp;
				swapped = 1;
			}
			current = current->next;
		}
	}
}

void print_and_free_list(t_perm_list *list)
{
	t_perm_list *temp;
	
	while (list)
	{
		puts(list->perm);
		temp = list;
		list = list->next;
		free(temp->perm);
		free(temp);
	}
}

// Heap's algorithm for generating permutations
void heap_permute(char *str, int size, int n, t_perm_list **list)
{
	if (size == 1)
	{
		add_to_list(list, str, n);
		return;
	}
	
	int i = 0;
	while (i < size)
	{
		heap_permute(str, size - 1, n, list);
		
		if (size % 2 == 1)
			swap_chars(&str[0], &str[size - 1]);
		else
			swap_chars(&str[i], &str[size - 1]);
		
		i++;
	}
}

void generate_permutations(char *str, int len, t_perm_list **list)
{
	heap_permute(str, len, len, list);
}

int main(int argc, char **argv)
{
	if (argc != 2 || !argv[1])
		return 1;
	
	char *input = argv[1];
	int len = str_len(input);
	
	if (len == 0)
		return 1;
	
	// Create a working copy of the string
	char *work_str = malloc(len + 1);
	if (!work_str)
		return 1;
	
	str_copy(work_str, input, len);
	
	t_perm_list *permutations = NULL;
	generate_permutations(work_str, len, &permutations);
	
	sort_list(&permutations);
	print_and_free_list(permutations);
	
	free(work_str);
	return 0;
}