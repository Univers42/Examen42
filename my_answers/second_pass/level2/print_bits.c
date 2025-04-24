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

void ft_print_bits(unsigned char octet)
{
    for (int i = 7; i >= 0; i--)
        write(1, ((octet >> i) & 1) ? "1" : "0", 1);
}

//the representation of the true ASCII TABLE
int main(void)
{
    printf("BIN  \t\tASCII        \tCHAR\n");
    printf("-----  \t\t--------  \t----\n");
    for (int octet = 31; octet < 128; octet++)
    {
        printf("\t%3d    \t", octet);
        ft_print_bits((unsigned char)octet);
        printf("    ");
        if (IS_PRINTABLE(octet))
            printf("\t%c", octet);
        else
            printf("\t.");
        printf("\n");
    }
    return 0;
}