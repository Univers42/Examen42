/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 11:17:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/17 14:03:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"

char *ft_strchr(char *str, int c)
{
    char *tmp;

    tmp = str;
    while (*tmp)
    {
        if (*tmp == (char)c)
            return (tmp);
        tmp++;
    }
    return  (NULL);
}

void *ft_memmove(void *dst, const void *src, size_t n)
{
    const unsigned char *s = (const unsigned char *)src;
    unsigned char *d;

    d = (unsigned char *)dst;
    if (d == s)
        return (dst);
    if (d < s)
    {
        while (n--)
            *d++ = *s++;
    }
    else
    {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return (dst);
}

void *ft_realloc(void *ptr, size_t oldsize, size_t newsize)
{
    unsigned char *new_buffer;
    size_t copy_size;

    if (newsize == 0)
        return (free(ptr), NULL);
    if (!ptr)
        return (malloc(newsize));
    new_buffer = malloc(newsize);
    if (!new_buffer)
        return (NULL);
    copy_size = oldsize < newsize ? oldsize : newsize;
    if (copy_size > 0)
        ft_memmove(new_buffer, ptr, copy_size);
    free(ptr);
    return ((void *)new_buffer);
}