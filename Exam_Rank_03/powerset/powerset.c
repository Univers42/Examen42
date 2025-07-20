#include "powerset.h"

void error_exit(void)
{
	exit(1);
}

t_subset *create_subset(int *elements, int size)
{
	t_subset *subset = malloc(sizeof(t_subset));
	if (!subset)
		error_exit();
	
	if (size > 0)
	{
		subset->elements = malloc(sizeof(int) * size);
		if (!subset->elements)
		{
			free(subset);
			error_exit();
		}
		
		int i = 0;
		while (i < size)
		{
			subset->elements[i] = elements[i];
			i++;
		}
	}
	else
		subset->elements = NULL;
	
	subset->size = size;
	subset->next = NULL;
	return subset;
}

void add_subset(t_subset **head, int *elements, int size)
{
	t_subset *new_subset = create_subset(elements, size);
	new_subset->next = *head;
	*head = new_subset;
}

void print_and_free_subsets(t_subset *head)
{
	t_subset *current = head;
	t_subset *temp;
	
	while (current)
	{
		int i = 0;
		while (i < current->size)
		{
			printf("%d", current->elements[i]);
			if (i < current->size - 1)
				printf(" ");
			i++;
		}
		printf("\n");
		
		temp = current;
		current = current->next;
		if (temp->elements)
			free(temp->elements);
		free(temp);
	}
}

void backtrack(int *arr, int n, int target, int index, 
			int *current, int current_size, t_subset **result)
{
	// If target sum is achieved, add current subset to result
	if (target == 0)
	{
		add_subset(result, current, current_size);
		return;
	}
	
	// If we've gone through all elements or target becomes negative
	if (index >= n)
		return;
	
	// Try including current element (if it doesn't make sum exceed target)
	if (arr[index] <= target)
	{
		current[current_size] = arr[index];
		backtrack(arr, n, target - arr[index], index + 1, 
				current, current_size + 1, result);
	}
	
	// Try excluding current element
	backtrack(arr, n, target, index + 1, current, current_size, result);
}

void find_subsets(int *arr, int n, int target, t_subset **result)
{
	int *current = malloc(sizeof(int) * n);
	if (!current)
		error_exit();
	
	backtrack(arr, n, target, 0, current, 0, result);
	
	free(current);
}

int parse_arguments(int argc, char **argv, int **numbers)
{
	if (argc < 2)
		return -1;
	
	int target = atoi(argv[1]);
	int count = argc - 2;
	
	if (count == 0)
	{
		*numbers = NULL;
		return target;
	}
	
	*numbers = malloc(sizeof(int) * count);
	if (!*numbers)
		error_exit();
	
	int i = 0;
	while (i < count)
	{
		(*numbers)[i] = atoi(argv[i + 2]);
		i++;
	}
	
	return target;
}

int main(int argc, char **argv)
{
	int *numbers;
	int target = parse_arguments(argc, argv, &numbers);
	int count = argc - 2;
	
	if (argc < 2)
		return 1;
	
	// Handle case where target is 0 and no numbers provided
	if (count == 0)
	{
		if (target == 0)
			printf("\n"); // Empty set sums to 0
		return 0;
	}
	
	t_subset *result = NULL;
	find_subsets(numbers, count, target, &result);
	
	print_and_free_subsets(result);
	
	if (numbers)
		free(numbers);
	
	return 0;
}