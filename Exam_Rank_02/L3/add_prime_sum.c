/*
Assignment name  : add_prime_sum
Expected files   : add_prime_sum.c
Allowed functions: write, exit
--------------------------------------------------------------------------------

Write a program that takes a positive integer as argument and displays the sum
of all prime numbers inferior or equal to it followed by a newline.

If the number of arguments is not 1, or the argument is not a positive number,
just display 0 followed by a newline.

Yes, the examples are right.

Examples:

$>./add_prime_sum 5
10
$>./add_prime_sum 7 | cat -e
17$
$>./add_prime_sum | cat -e
0$
$>
*/
# include <unistd.h>
# include <stdlib.h>
#include <unistd.h>

int	ft_atoi(const char *str)
{
    int sign = 1; 
    int result = 0;

    while (*str == ' ' || (*str >= 9 && *str <= 13))
        	str++;
	if (*str == '-')
		sign = -1;
	if (*str == '-' || *str == '+')
		str++;

    while(*str >= '0' && *str <= '9')
        result = result * 10 + *str++ - '0'; 
    return (result *sign); 
}
void	ft_putnbr(int n)
{
	char number[] = "0123456789";

	if(n >= 10)
		ft_putnbr (n / 10);
	write(1, &number[n % 10], 1); 
}

int	main(int argc, char *argv[])
{
    int sum = 0;
    
    if(argc == 2)
    {
	    int n = ft_atoi(argv[1]);
        int sum = 0;

        for (int i = 2; i <= n; ++i)
        {
            int is_prime = 1;
            for (int j = 2; j * j <= i; ++j)
            {
                if (i % j == 0)
                {
                    is_prime = 0;
                    break;
                }
            }
            if (is_prime)
                sum += i;
        }
        ft_putnbr(sum);
    }
    if (argc != 2)
        putnbr(0); 
    write(1, "\n", 1);
    return 0;
}
