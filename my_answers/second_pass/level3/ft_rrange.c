#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#################################### UTILS FOR EXERCICES ######################################
# define WRITE_CHAR(c) (write(1,(c),1))
# define NEWLINE() (WRITE_CHAR("\n"))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define ISUPPER(c) (((c) & ~0x1F) == 0x40)
# define ISLOWER(c) (((c) & ~0x1F) == 0x60)
# define ISALPHA(c) (ISUPPER(c) || ISLOWER(c))
# define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

//################################## DEFINES FOR SPEFICIC EXERCICE ##############################

int ft_abs(int nbr)
{
    return ((nbr  < 0) ? -nbr : nbr);
}

int *ft_range(int start, int end)
{
    int *tab;
    int size = ft_abs(end - start) + 1;
    tab = malloc(sizeof(int) * size);
    if(!tab)
        return (NULL);
    for (int i = 0; i < size; i++)
        tab[i] = (start > end) ? end + i : end - i;
    return (tab);
}

int main(int argc, char **argv)
{
    if(argc != 3)
        return (NEWLINE(), 1);
    int end =atoi(argv[2]);
    int start=atoi(argv[1]);
    int *tab = ft_range(start,end);
    int size = ft_abs(end - start) + 1;
    for (int i = 0; i < size; i++)
        printf("%d\t", tab[i]);
    return (0);
}