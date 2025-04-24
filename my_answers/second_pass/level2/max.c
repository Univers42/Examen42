#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ========================= UTILS FOR EXERCICE ==============================

# define WRITE_CHAR(x, size) (write(1, &(x), size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) (((x) & ~0x1F) == 0x40)
# define IS_LOWER(x) (((x) & ~0x1F) == 0x60)
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

// ========================= SPECIFIC FOR EXERCICE ==============================
# define BUFF 5

int ft_max(int *tab, int size)
{
    int max = *tab;
    int i = 0;
    if (size == 0)
        return (0);
    for (int i = 1;i < size;i++)
    {
        int mask = -(tab[i] > max);
        max = (mask & tab[i]) | (~mask & max);
    }
    return (max);
}

int main(void)
{
    int *tab;
    tab = malloc(sizeof(int) * BUFF);
    srand(time(NULL));
    for (int i = 0; i < BUFF; i++)
        tab[i] = rand() % 100;
    for (int i = 0; i < BUFF; i++)
        printf("tab[%d] == > %d\n", i, tab[i]);
    int max = ft_max(tab, BUFF);
    printf("\e[40m%d\n", max);
    return (0);
}
