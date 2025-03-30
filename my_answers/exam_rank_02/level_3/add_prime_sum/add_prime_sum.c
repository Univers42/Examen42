#include <unistd.h>

int ft_atoi(char *str)
{
    int num = 0;
    
    while (*str >= '0' && *str <= '9')
    {
        // Multiply by 10 using bitwise shifts (10 = 8 + 2 = 2^3 + 2^1)
        num = (num << 3) + (num << 1) + (*str - '0');
        str++;
    }
    
    return num;
}

void ft_putnbr(int n)
{
    char digit;
    
    if (n >= 10)
        ft_putnbr(n / 10);
    digit = (n % 10) + '0';
    write(1, &digit, 1);
}


int is_prime(int n)
{
    int i;

    if (n < 2)
        return (0);
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return (0);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "0\n", 2);
        return (0);
    }
    int n = ft_atoi(argv[1]);
    int sum = 0;
    for (int i = 2; i <= n; i++)
        if (is_prime(i))
            sum += i;
    ft_putnbr(sum);
    write(1, "\n", 1);
    return (0);
}
