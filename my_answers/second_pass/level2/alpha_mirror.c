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

typedef enum s_mirror
{
    MIRROR_LOWER = 'a' + 'z',
    MIRROR_UPPER ='A' + 'Z',
}t_mirror;

# define DELTA_MIRROR(mir_type, x) (mir_type - x)

void ft_alpha_mirror(char *str)
{
    while(*str)
    {
        if (!IS_ALPHA(*str))
            WRITE_CHAR(*str, 1);
        else 
        {
            *str = IS_UPPER(*str) ? DELTA_MIRROR(MIRROR_UPPER, *str) : DELTA_MIRROR(MIRROR_LOWER, *str);
            WRITE_CHAR(*str, 1);
        }
        str++;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (1);
    }
    char *str = argv[1];
    ft_alpha_mirror(str);
    return (0);
}
