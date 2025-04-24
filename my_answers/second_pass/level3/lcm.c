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

unsigned int HCF(unsigned int a, unsigned int b)
{
    if (b == 0)
        return a;
    return (HCF(b, a % b));
}

unsigned int ft_lcm(unsigned int a, unsigned int b)
{
    return ((a * b) / HCF(a, b));
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return(NEWLINE(), 1);
    unsigned int a = atoi(argv[1]);
    unsigned int b = atoi(argv[2]);
    printf("%d\n", HCF(a, b));
    return (0);
}

