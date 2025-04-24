#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#################################### UTILS FOR EXERCICES ######################################
# define WRITE_CHAR(c, size) (write(1,&(c),size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define ISUPPER(c) (((c) & ~0x1F) == 0x40)
# define ISLOWER(c) (((c) & ~0x1F) == 0x60)
# define ISALPHA(c) (ISUPPER(c) || ISLOWER(c))
# define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

//################################## DEFINES FOR SPEFICIC EXERCICE ##############################

void ft_putnbr(unsigned int nbr)
{
    char c;
    if (nbr > 9)
        ft_putnbr(nbr / 10);
    c = nbr % 10 + '0';
    WRITE_CHAR(c, 1);
}

unsigned int isprime(unsigned int n)
{
    int i = 2;
    while(i * i <= n)
    {
        if (n % i == 0)
            return (0);
        i++;
    }
    return (1);
}

unsigned int ft_add_prime_sum(unsigned int n)
{
    unsigned int count;

    count = 0;
    for (int i = 2; i <= n; i++)
    {
        if (isprime(i))
            count += i;
    }
    return (count);
}


int main(int argc, char **argv)
{
    if (argc != 2)
        return(NEWLINE(), 1);
    int str = atoi(argv[1]);
    unsigned int sum = ft_add_prime_sum(str);
    ft_putnbr(sum);
    NEWLINE();
    return (0);
}