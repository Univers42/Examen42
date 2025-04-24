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

int conv_char(char c, int str_base)
{
    int val = -1;
    if (ISDIGIT(c))
        val = c - '0';
    else if (ISUPPER(c))
        val = c - 'A' + 10;
    else if (ISLOWER(c))
        val = c - 'a' + 10;
    if (val >= 0 && val < str_base)
        return val;
    return -1;
}

unsigned int ft_atoi_base(char *str, int str_base)
{
    int is_negative = 1;
    int result = 0;
    int val;

    while (IS_SPACE(*str))
        str++;
    if (*str == '-' || *str == '+')
        is_negative = (*str == '-') ? -1 : 1, str++;
    if (str_base < 2 || str_base > 16)
        return 0;
    while ((val = conv_char(*str, str_base)) != -1)
    {
        result = result * str_base + val;
        str++;
    }
    return (result * is_negative);
}


int main(int argc, char **argv)
{
    if (argc != 3)
        return (NEWLINE(), 1);

    char *str = argv[1];
    int base = atoi(argv[2]);
    int result = ft_atoi_base(str, base);

    printf("%d\n", result);
    return 0;
}