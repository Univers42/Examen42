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

void ft_print_hex(unsigned int nb)
{
    char *str = "0123456789abcdef";
    if (nb > 16)
        ft_print_hex(nb / 16);
    write(1, &str[nb % 16], 1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return(NEWLINE(), 1);
    int number = atoi(argv[1]);
    ft_print_hex(number);
    return (0);
}

