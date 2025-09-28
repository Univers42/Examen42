#include <stdio.h>
#include <stdlib.h>
#include "ft_list.h"

// Function to apply to each element
void print_data(void *data)
{
    printf("%s\n", (char *)data);
}

// Function to create a new list node
t_list *ft_create_elem(void *data)
{
    t_list *new = malloc(sizeof(t_list));
    if (!new)
        return NULL;
    new->data = data;
    new->next = NULL;
    return new;
}

int main()
{
    // Creating a simple linked list
    t_list *list = ft_create_elem("Hello");
    list->next = ft_create_elem("42");
    list->next->next = ft_create_elem("World");

    // Apply ft_list_foreach
    ft_list_foreach(list, print_data);

    return 0;
}
