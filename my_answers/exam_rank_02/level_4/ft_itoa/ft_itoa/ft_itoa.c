#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ft_log10(int n)
{
    int count = 0;
    if (n <= 0)
        ++count;
    while(n)
    {
        ++count;
        n /= 10;
    }
    return (count);
}

char *ft_itoa(int n)
{
    int len = ft_log10(n);
    char *str = malloc(len + 1);
    int is_negative;
    unsigned int abs_num;
    int i;

    if (!str) return NULL;
    str[len] = '\0';
    is_negative = (n >> 31) & 1;
    abs_num = n;
    abs_num = (abs_num ^ -is_negative) + is_negative;
    
    if ((n ^ 0) == 0)
    {
        str[0] = '0';
        return (str);
    }
    
    i = len - 1;
    while (abs_num)
    {
        *(str + i--) = (abs_num % 10) + '0';
        abs_num /= 10;
    }
    if (is_negative)
        str[0] = '-';
    return (str);
}

int main(void)
{
    int number = -42445;
    char *str = ft_itoa(number);
    printf("%s\n", str);
    free(str);
    
    // Test with negative number
    number = -12345;
    str = ft_itoa(number);
    printf("%s\n", str);
    free(str);

    number = 0;
    str = ft_itoa(number);
    printf("%s\n", str);
    free(str);
    return 0;
}
