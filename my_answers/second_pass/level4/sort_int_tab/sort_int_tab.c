#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER 30
// selection sort
void ft_swap(int *a, int *b)
{
    (*a ^= *b),(*b ^= *a),(*a ^= *b);
}
void sort_int_tab(int *tab, unsigned int size)
{
    unsigned int min_idx;
    for(int i = 0; i < size - 1; i++)
    {
        min_idx = i;
        for (int j = i + 1; j < size; j++)
        {
            if (tab[min_idx] > tab[j])
                min_idx = j;
        }
        if (min_idx != i)
            ft_swap(&tab[i], &tab[min_idx]);
    }
}

int main(int argc, char **argv)
{
    int *tab;
    tab = malloc(sizeof(int) * BUFFER);
    if (!tab)
        return (1);
    srand(time(NULL));
    for (int i = 0; i < BUFFER; i++)
        tab[i] = rand() % 1000;
    // see if the filling bucket have worked
    for (int i = 0; i < BUFFER; i++)
        printf("tab[%d] == > %d\n",i,tab[i]);
    sort_int_tab(tab, BUFFER);
    for (int i = 0; i < BUFFER; i++)
        printf("\e[33mtab[%d] ==> %d\n\e[0m", i, tab[i]);
    return (0);
}