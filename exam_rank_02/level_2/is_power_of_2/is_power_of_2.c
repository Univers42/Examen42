#include <stdio.h>
#include <unistd.h>
#define ISDIGIT(c) (c >= '0' && c <= '9')

int ft_is_power_of_2(unsigned int n)
{
    return((n != 0) && (n & (n - 1)) == 0);
}

int ft_atoi(char *str)
{
    int res;
    
    res = 0;
    while(ISDIGIT(*str))
        res = (res << 3) + (res << 1) + (*str++ - '0');
    return (res);
}

int main(int argc, char **argv)
{
    unsigned int number;
    if(argc != 2)
        return (write(1,"\n",1));
    number = (unsigned int)ft_atoi(argv[1]);
    if(ft_is_power_of_2(number))
        printf("the number is a power of 2");
    else
        printf("The number is not a power of  2");
    return (0);
}
