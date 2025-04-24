#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================= UTILS FOR EXERCICE ==============================

#define WRITE_CHAR(x, size) (write(1, (x), size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) (((x) & ~0x1F) == 0x40)
# define IS_LOWER(x) (((x) & ~0x1F) == 0x60)
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))
// ========================= SPECIFIC FOR EXERCICE ==============================

typedef struct s_maps
{
    unsigned int seen[8];
    unsigned int printed[8];
}t_maps;

#define VECTOR_BIT_MAP(bitmap, c) (bitmap[(unsigned char)(c) / 32] |= (1U << ((unsigned char)(c) % 32)))
#define CHECK_BIT_MAP(bitmap, c)  (bitmap[(unsigned char)(c) / 32] &  (1U << ((unsigned char)(c) % 32)))

void ft_inter(char *str, char *set)
{
    t_maps vector_bit = {{0}, {0}};
    while (*set)
    {
        unsigned char ch = *set++;
        VECTOR_BIT_MAP(vector_bit.seen, ch);
    }
    while (*str)
    {
        if (CHECK_BIT_MAP(vector_bit.seen, *str) && !CHECK_BIT_MAP(vector_bit.printed, *str))
        {
            WRITE_CHAR(str, 1);
            VECTOR_BIT_MAP(vector_bit.printed, *str);
        }
        str++;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        NEWLINE();
        return (1);
    }
    char *str = argv[1];
    char *set = argv[2];
    ft_inter(str, set);
    return (0);
}
