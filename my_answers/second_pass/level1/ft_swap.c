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

void ft_swap(int *a, int *b)
{
    (*a ^= *b), (*b ^= *a), (*a ^= *b);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        write(1, "\n", 1);
        return (1);
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    
    int *ptr1 = &a;
    int *ptr2 = &b;
    printf("original order: a = %d \t b = %d\n", a, b);
    ft_swap(ptr1, ptr2);
    printf("\e[40m new order : a = %d \t b = %d \n", *ptr1, *ptr2);
    return (0);
}
