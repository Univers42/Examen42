#include <stdlib.h>

int *ft_rrange(int start, int end)
{
    int size = (end ^ start) >= 0 ? (end - start) + 1 : (start - end) + 1;
    int *arr = (int *)malloc(size * sizeof(int));
    if (!arr)
        return (NULL);
    
    for (int i = 0; i < size; i++)
        arr[i] = end > start ? (end - i) : (end + i);
    
    return arr;
}
