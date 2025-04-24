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
    while(*str)
        str++;
    return (str - temp);
}

void ft_last_word(char *str)
{
    char *end;
    int length;
    int count;

    length = ft_strlen(str);
    end = str + length - 1;
    count = 0;
    while(IS_SPACE(*end))
        end--;
    while(end >= str)
    {
        if (IS_SPACE(*end))
        {
            WRITE_CHAR((end + 1),count);
            break ;
        }
        end--;
        count++;
    }
    NEWLINE();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    ft_last_word(str);
    return (0);
}
