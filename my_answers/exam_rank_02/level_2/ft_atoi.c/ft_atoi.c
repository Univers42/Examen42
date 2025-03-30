/**
 * ft_atoi.c [Strength reduction]
 * *Strength reduction is a compiler optimization technique that replaces expensive operation with cheaper ones
 * Example : "    -123abc"
 * 1. Skip leading spaces
 * 2. Check for sign
 * 3. Convert digits to integer
 * 
 * This implementation uses bitwise operations to convert characters to integers.
 * Digit 1:
 * result = (0 << 3) + (0 << 1) + (1 - '0') = 0 + 0 + 1 = 1
 * in binary : (0000 << 3) + (0000 << 1) + 1
 * result = 0 + 0 +1 = 1
 * 
 * Digit 2:
 * result = (1 << 3) + (1 << 1) + (2 - '0') = 8 + 2 = 10
 * in binary : (0001 << 3) + (0001 << 1) + 2
 * This equals to : (0000 1000) +(0000 0010) + 2
 * resutl = 12
 * 
 * Digit 3: 
 * Calculate : (12 << 3) + (12 << 1) + ('3' - '0')
 * in binary : (1100 << 3) + (1100 << 1) + 3)
 * This equals to : (1100 0000) + (0011 0000) + 0000 0011
 * result = 96 + 24 + 3 = 123
 * 
 */
int ft_atoi(const char *str)
{
    int result = 0;
    int sign = 1;

    while (*str == ' ' || (*str >= 9 && *str <= 13))
        str++;
    if (*str == '-' || *str == '+')
    {
        sign = (*str == '-') ? -1 : 1;
        str++;
    }
    while (*str >= '0' && *str <= '9')
    {
        result = (result << 3) + (result << 1) + (*str - '0');
        str++;
    }
    return (result * sign);
