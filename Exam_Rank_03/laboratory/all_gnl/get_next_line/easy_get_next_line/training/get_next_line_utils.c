#include "get_next_line.h"

void    *ft_memmove(void *dst, const void *src, size_t nbytes)
{
    const unsigned char *s = (const unsigned char *)src;
    unsigned char *d;

    d = (unsigned char *)dst;
    if (s == d)
        return (dst);
    if (d < s)
    {
        while (nbytes--)
            *d++ = *s++;
    }
    else
    {
        d += nbytes;
        s += nbytes;
        while (nbytes--)
            *--d = *--s;
    }
    return (dst);
}

void    *ft_memchr(void *src, int c, size_t nbytes)
{
    unsigned char *ptr = (unsigned char *)src;

    while (nbytes--)
    {
        if (*ptr == (unsigned char)c)
            return(ptr);
        ptr++;
    }
    return (NULL);
}

void    *ft_realloc(void *src, size_t old, size_t new)
{
    char    *tmp;
    size_t  copy_size;

    if (!new)
        return (NULL);
    tmp = malloc(sizeof(char) * new);
    if (!tmp)
        return (NULL);
    if (src)
    {
        if (old < new)
            copy_size = old;
        else
            copy_size = new;
        ft_memmove(tmp, src, copy_size);
        free(src);
    }
    return (tmp);
}