#include "ft_list.h"

int ft_list_size(t_list *begin_list)
{
    int size = 0;
    t_list *current = begin_list;

    while (current)
    {
        size++;
        current = current->next;
    }
    return size;
}