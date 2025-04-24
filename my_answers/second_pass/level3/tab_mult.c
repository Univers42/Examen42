#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#################################### UTILS FOR EXERCICES ######################################
# define WRITE_CHAR(c) (write(1,&(c),1))
# define NEWLINE() (WRITE_CHAR("\n"))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define ISUPPER(c) (((c) & ~0x1F) == 0x40)
# define ISLOWER(c) (((c) & ~0x1F) == 0x60)
# define ISALPHA(c) (ISUPPER(c) || ISLOWER(c))
# define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

//################################## DEFINES FOR SPEFICIC EXERCICE ##############################

void ft_putstr(char *str)
{
    int count;
    char *temp;
    temp = str;
    count = 0;
    while (*str)
        (count++, str++);
    write(1,temp,count);
}

void ft_putnbr(unsigned int nbr)
{
    char c;
    if (nbr > 9)
        ft_putnbr(nbr / 10);
    c = nbr % 10 + '0';
    WRITE_CHAR(c);
}

void ft_tab_mult(int nb)
{
    for (int i = 1; i < 10; i++)
    {
        ft_putnbr(i);
        ft_putstr(" x ");
        ft_putnbr(nb);
        ft_putstr(" = ");
        ft_putnbr(i * nb);
        NEWLINE();
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return(NEWLINE(), 1);
    int number = atoi(argv[1]);
    ft_tab_mult(number);
    return (0);
}

