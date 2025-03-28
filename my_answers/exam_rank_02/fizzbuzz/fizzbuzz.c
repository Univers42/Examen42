#include <unistd.h>

/**
 * The mathematical rule to check if a number is a multiple of another number is simple: a number
 * `a` is a multiple of another number `b` if the remainder of `a % b` is zero.
 */
void ft_putstr(char *str)
{
    while (*str)
        write(1, str++, 1);
}

void ft_putnbr(int n)
{
    char c;

    if (n >= 10)
        ft_putnbr(n/10);
    c = (n % 10 ) + '0';
    write(1, &c, 1);
}

void print_number(int num)
{
    ft_putnbr(num);
    ft_putstr("\n");
}

void print_fizz(int num)
{
    ft_putstr("fizz\n");
}

void print_buzz(int num)
{
    ft_putstr("buzz\n");
}

void print_fizzbuzz(int num)
{
    ft_putstr("fizzbuzz\n");
}

void fizzbuzz(void)
{
    typedef void (*state_handler)(int);
    state_handler state_handlers[4] = 
    {
        print_number,
        print_fi,
	print_buzz,
        print_fizzbuzz
    };
    int i = 1; // Fixed: initialized i to 1
    int state;
    
    while (i <= 100)
    {
        state = 0;
        if (i % 3 == 0)
            state |= 1;
        if (i % 5 == 0)
            state |= 2;
        state_handlers[state](i);
        i++;
    }
}

int main(void)
{
    fizzbuzz();
    return (0);
}
