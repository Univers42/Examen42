#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================= UTILS FOR EXERCICE ==============================

# define WRITE_CHAR(x, size) (write(1, &(x), size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) (((x) & ~0x1F) == 0x40)
# define IS_LOWER(x) (((x) & ~0x1F) == 0x60)
#define IS_DIGIT(x) ((x) >= '0' && (x) <= '9')
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

// ========================= SPECIFIC FOR EXERCICE ==============================

int ft_atoi(char *str)
{
    int is_negative = 0;
    int res = 0;
    while(IS_SPACE(*str))
        str++;
    if (*str == '+' || *str == '-')
        is_negative = (*str++ ^ '-') == 0;
    while (IS_DIGIT(*str))
        res = (res << 3) + (res << 1) + (*str++ - '0');
    return (((~res + 1) & -is_negative) | (res & ~-is_negative));
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    printf("%d\n", ft_atoi(str));
    return (0);
}
