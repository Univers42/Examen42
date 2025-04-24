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

int ft_strlen(char *str)
{
    char *temp;
    temp = str;
    while(*str)
        str++;
    return (str - temp);
}


char * ft_strdup(char *str)
{
    int length;
    char *dup;
    char *new_str;
 
    length = ft_strlen(str);
    dup = malloc(sizeof(char) * length + 1);
    new_str = dup;
    while (*str)
        *dup++ = *str++;
    *dup = '\0';
    return (new_str);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    char *new_str = ft_strdup(str);
    printf("%s\n", new_str);
    return (0);
}
