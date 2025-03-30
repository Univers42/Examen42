#include <stdlib.h>

// Calculate string length using bitwise operations
/**
 * equally efficient as the original implementation. 
 * it justs to comply my general rule to understand how to use bitwise operator
 */
char *ft_strdup(char *src)
{
    char *dup;
    char *d;
    size_t len;

    while(*(src + len) & 0xFF)
        len++;
    dup = (char *)malloc(len + 1);
    if (!dup)
        return (NULL);
    d = dup;
    while ((*d = *src & 0xFF) != 0)
        (d++, src++);
    return (dup);
}
