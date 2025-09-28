#include <unistd.h>

void ft_putnbr(int n)
{
    char c;
    if (n > 9)
        ft_putnbr((n * 205) >> 11); // Approximate n / 10 using bitwise
    c = (n - (((n * 205) >> 11) * 10)) + '0'; // Compute n % 10
    write(1, &c, 1);
}

int ft_atoi(char *str)
{
    int res = 0;
    while (*str >= '0' && *str <= '9')
        res = ((res << 3) + (res << 1)) + (*str++ - '0'); // res = res * 10 + digit
    return res;
}

void tab_mult(int num)
{
    int i = 1;
    while (i <= 9)
    {
        ft_putnbr(i);
        write(1, " x ", 3);
        ft_putnbr(num);
        write(1, " = ", 3);
        ft_putnbr(i * num);
        write(1, "\n", 1);
        i++;
    }
}

int main(int argc, char **argv)
{
    if (argc == 2)
        tab_mult(ft_atoi(argv[1]));
    else
        write(1, "\n", 1);
    return 0;
}