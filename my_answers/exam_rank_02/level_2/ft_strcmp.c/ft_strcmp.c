#include <stdio.h>
// Use bitwise XOR to compare two strings
// use substraction for the final result.
/**
 * How does that work?
 * we cast to unsigned char to avoid negative values
 * we use XOR to compare the two strings
 * if they are equal, the result will be 0
 * if they are not equal, the result will be the difference between the two characters
 * Example :
 * s1 = "Hello"
 * s2 = "Hello"
 * s1[0] = 'H' (72)
 * s2[0] = 'H' (72)
 * s1[1] = 'e' (101)
 * s2[1] = 'e' (101)
 * s1[2] = 'l' (108)
 * s2[2] = 'l' (108)
 * s1[3] = 'l' (108)
 * s2[3] = 'l' (108)
 * s1[4] = 'o' (111)
 *  s2[4] = 'o' (111)
 * * s1[5] = '\0' (0)
 * s2[5] = '\0' (0)
 * * The loop will terminate when we reach the end of the string
 * * The result will be 0, since the two strings are equal
 * * If we compare "Hello" and "World", the result will be -15
 * * since 'H' (72) - 'W' (87) = -15
 * * * If we compare "Hello" and "Hello, World!", the result will be -44
 * * since 'H' (72) - '\0' (0) = -44
 * 'H' (72) ^ 'd' (100)
 * binary representation: 00100111 ^ 01100100 = 01000011
 * the result is not 00000000 so it is not equal
 * 'E' (69) ^ 'E' (69)
 * binary representation: 01000101 ^ 01000101 = 00000000
 * the result is 00000000 so it is equal
 */
int ft_strcmp(char *s1, char *s2)
{
    unsigned char *us1 = (unsigned char *)s1;
    unsigned char *us2 = (unsigned char *)s2;
    while(*us1 && (*us2 == *us1))
    {
        us1++;
        us2++;
    }
    if (*us1 ^ *us2)
        return (*us1 - *us2);
    return (0);
}


int main()
{
    char *str1 = "Hello";
    char *str2 = "Hello";
    char *str3 = "World";
    char *str4 = "Hello, World!";
    char *str5 = "Hello, World!";
    printf("%d\n", ft_strcmp(str1, str2));
    printf("%d\n", ft_strcmp(str1, str3));
    printf("%d\n", ft_strcmp(str1, str4));
    printf("%d\n", ft_strcmp(str1, str5));
    printf("%d\n", ft_strcmp(str2, str3));
    printf("%d\n", ft_strcmp(str2, str4));
    printf("%d\n", ft_strcmp(str2, str5));
    printf("%d\n", ft_strcmp(str4, str5));
    return (0);
}