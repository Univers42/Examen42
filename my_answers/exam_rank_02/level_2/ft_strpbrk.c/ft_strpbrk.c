#include <stdlib.h>
char *ft_strpbrk(const char *s1, const char *s2)
{
    unsigned char bitmap[32] = {0};
    if (!*s2)
        return NULL;
    while (*s2)
    {
        unsigned char uc = (unsigned char)*s2;
        bitmap[uc >> 3] |= (1 << (uc & 7));
        s2++;
    }
    while (*s1)
    {
        unsigned char uc = (unsigned char)*s1;
        if (bitmap[uc >> 3] & (1 << (uc & 7)))
            return (char *)s1;
        s1++;
    }
    return NULL;
}

/*
#include <stdint.h>
#include <stdio.h>

char *ft_strpbrk(const char *s1, const char *s2)
{
    uint32_t bit_map[8] = {0};
    
    while(*s2)
    {
        bit_map[(unsigned char) *s2 / 32] |= (1U << ((unsigned char)*s2 %32));
        s2++;
    }
    while(*s1)
    {
        if (bit_map[(unsigned char) *s1 / 32] & (1U << ((unsigned char)*s1 % 32)))
            return ((char *)s1);
        s1++;
    }
    return (NULL);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    char *ret = ft_strpbrk(argv[1], "abcdef");
    printf("%s\n", ret);
    return (0);
}
*/
