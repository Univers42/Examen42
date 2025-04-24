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
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

// ========================= SPECIFIC FOR EXERCICE ==============================

int position = 0;
int ft_strcmp(char *str1, char *str2)
{
    while(*str1 && *str2)
    {
        if ((*str1 ^ *str2) != 0)
            return ((unsigned char)*str1 - (unsigned char)*str2);
        str1++;
        str2++;
        position++;
    }
    return (0);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    char *str2 = argv[2];
    printf("%d(%c) - %d(%c) = %d\n",
            str[position],
            str[position],
            str2[position],
            str2[position],
            ft_strcmp(str, str2));
    return (0);
}
