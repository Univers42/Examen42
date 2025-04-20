#include <stdlib.h>
#include <stdio.h>

int ft_abs(int number)
{
    int mask = number >> (sizeof(int) * 8 -1);
    return ((number + mask) ^ mask);
}

int *ft_rrange(int start, int end)
{
    int size = ft_abs(end - start) + 1;
    int *tab = malloc(sizeof(int) * size);
    if(!tab)
        return (NULL);
    for (int i = 0; i < size; i++)
        tab[i] = (end <= start) ? end + i : end - i;
    return (tab);
}

int main(int argc, char **argv)
{
    if(argc != 3)
        return 1;
    
    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int size = end -start + 1;
    int *arr = ft_rrange(start, end);
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    free(arr);
    return (0);
}
