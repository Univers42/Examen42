/*
Assignment name  : ft_atoi_base
Expected files   : ft_atoi_base.c
Allowed functions: None
--------------------------------------------------------------------------------

Write a function that converts the string argument str (base N <= 16)
to an integer (base 10) and returns it.

The characters recognized in the input are: 0123456789abcdef
Those are, of course, to be trimmed according to the requested base. For
example, base 4 recognizes "0123" and base 16 recognizes "0123456789abcdef".

Uppercase letters must also be recognized: "12fdb3" is the same as "12FDB3".

Minus signs ('-') are interpreted only if they are the first character of the
string.

Your function must be declared as follows:

int	ft_atoi_base(const char *str, int str_base);
*/

#include <unistd.h>

int ft_value(char c)
{
    if (c >= '0' && c <= '9')
        return(c - '0'); 
    else if (c >= 'a' && c <= 'f')
        return(c - 'a' + 10);
    else if (c >= 'A' && c <= 'F')
        return(c - 'A' + 10);
    return -1; 

}

int	ft_atoi_base(const char *str, int str_base)
{
    if (str_base < 2 || str_base > 16) 
        return 0;

    int result = 0;
    int sign = 1; 

    while(*str == ' ' || (*str >= 9 && *str <= 13 ))
        str++;
    if ( *str == '-') 
        sign = -1;
    if ( *str == '-' || *str == '+')
        str++; 

    while (*str)
    {
        int value = ft_value(*str); 

        if (value < 0 || value >= str_base)
            break; 
        result = result * str_base + value; 
        str++; 
    }
    return (result * sign);     
}
