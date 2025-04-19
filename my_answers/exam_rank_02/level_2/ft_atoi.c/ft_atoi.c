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
#include <unistd.h>
#include <stdio.h>
# define IS_SPACE(c) (((c) ^ '\t') == 0 || ((c) ^ ' ') == 0)
# define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
# define NEWLINE() (write(1,"\n",1))

int ft_atoi(char *number)
{
    int result = 0;
    int is_negative = 0;
    
    while(IS_SPACE(*number))
        number++;
    if((*number ^ '-') == 0 || (*number ^ '+') == 0)
        is_negative = (*number++ ^ '-') == 0;
    while(IS_DIGIT(*number))
        result = (result << 3) + (result << 1) + (*number++ - '0');
    return (((~result + 1) & -is_negative) | (result & ~-is_negative));
}

int main(int argc, char **argv)
{
    int number;
    if(argc != 2)
        return (NEWLINE(), 1);
    number = ft_atoi(argv[1]);
    printf("%d\n", number);
}
