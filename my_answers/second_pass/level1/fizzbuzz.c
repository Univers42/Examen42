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
# define FIZZBUZZ() (WRITE_CHAR("fizzbuzz", 8))
# define FIZZ() (WRITE_CHAR("fizz", 4))
# define BUZZ() (WRITE_CHAR("buzz", 4))

void ft_fizzbuzz(int n)
{
    (void)n;
    FIZZBUZZ();
}
void ft_fizz(int n)
{
    (void) n;
    FIZZ();
}
void ft_buzz(int n)
{
    (void) n;
    BUZZ();
}

void ft_putnbr(int n)
{
    char c;
    if (n > 9)
        ft_putnbr(n / 10);
    c = n % 10 + '0';
    WRITE_CHAR(c, 1);
}

void jump_table(void)
{
    typedef void (*handlers)(int);
    unsigned int state;

    handlers state_handlers[4] =
    {
        ft_putnbr,
        ft_fizz,
        ft_buzz,
        ft_fizzbuzz,
    };
    
    for (int i = 0; i <= 100; i++)
    {
        state = 0;
        if (i % 3 == 0)
            state |= 1;
        if (i % 5 == 0)
            state |= 2;
        state_handlers[state](i);
        NEWLINE();
    }
}

int main(void)
{
    jump_table();
    return (0);
}
