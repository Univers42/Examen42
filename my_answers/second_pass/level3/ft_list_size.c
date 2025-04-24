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
typedef struct  s_list
{
    struct s_list *next;
    void *data;
}               t_list;

int     ft_list_size(t_list *begin_list)
{
    int count = 0;
    while(begin_list)
    {
        count++;
        begin_list = begin_list->next;
    }
    return (count);
}
#define NODES 13

int main(void)
{
    t_list tab[NODES];
    srand(time(NULL));
    for (int i = 0; i < NODES; i++)
    {
           tab[i].data = (void *)(long)(rand() % 100);
           tab[i].next = (i < NODES - 1) ? &tab[i + 1] : NULL;
    }
    printf("%d\n",ft_list_size(&tab[0]));
    return (0);
}

