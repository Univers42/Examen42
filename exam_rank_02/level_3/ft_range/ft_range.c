#include <stdlib.h>
#include <stdio.h>

int ft_abs(int n)
{
    int mask = n >> (sizeof(int) * 8 - 1);
    return ((n + mask) ^ mask);
}

int *ft_range(int start, int end)
{
    int *tab;
    int size;

    size = ft_abs(end - start) + 1;
    tab = malloc(sizeof(int) * size);
    if(!tab)
        return NULL;
    for(int i = 0; i < size; i++)
        tab[i] = (start <= end) ? start + i : start - i;
    return (tab);
}

int main(void) {
    int a = 1;
    int b = 5;
    int *arr = ft_range(a, b);
    int size = ft_abs(b - a) + 1;
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
    free(arr);
    return 0;
}
