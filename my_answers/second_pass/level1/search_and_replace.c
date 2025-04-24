#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define WRITE_CHAR(x, size) (write(1, x, size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) ((x | ~0x20) == 0)
# define IS_LOWER(x) ((x & 0x20) == 0)
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

void ft_search_and_replace(char *str, char to_find, char to_replace)
{
    while (*str)
    {
        *str = (*str == to_replace) ? WRITE_CHAR(&to_find, 1) : WRITE_CHAR(str, 1);
        str++;
    }
    NEWLINE();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    char to_find = argv[2][0];
    char to_replace = argv[3][0];
    ft_search_and_replace(str, to_find, to_replace);
    return (0);
}
