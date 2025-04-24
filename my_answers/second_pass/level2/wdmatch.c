#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================= UTILS FOR EXERCICE ==============================

# define WRITE_CHAR(x, size) (write(1, x, size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) (((x) & ~0x1F) == 0x40)
# define IS_LOWER(x) (((x) & ~0x1F) == 0x60)
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

// ========================= SPECIFIC FOR EXERCICE ==============================

int ft_strlen(char *str)
{
    char *temp;
    temp = str;
    while(*temp++);
    return (temp - str -1);
}

void ft_wdmatch(char *str, char *set)
{
    char *wd = str;
    int size = ft_strlen(str);
    while(*str && *set)
        str += *set++ == *str;
    if (*str == '\0')
        WRITE_CHAR(wd, size);
    NEWLINE();
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    char *set = argv[2];
    ft_wdmatch(str, set);
    return (0);
}
