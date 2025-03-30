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