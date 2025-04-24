#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define WRITE_CHAR(x, size) (write(1, &(x), size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) ((x | ~0x20) == 0)
# define IS_LOWER(x) ((x & 0x20) == 0)
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

int ft_strlen(char *str)
{
    char *temp;
    
    temp = str;
    while(*temp)
        temp++;
    return (temp - str);
}

void ft_rev_print(char *str)
{
    char *end;
    char *start;

    start = str;
    end = str + ft_strlen(str) - 1;
    while(end >= start)
        (WRITE_CHAR(*end, 1), end--);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    ft_rev_print(str);
    return (0);
}
