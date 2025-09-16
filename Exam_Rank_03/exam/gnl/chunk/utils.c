/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 19:22:32 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/16 20:45:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"
#include <stddef.h>

void *memmove(void *dest, const void *src1, size_t n)
{
	const unsigned char *s = (const unsigned char*)src1;
	unsigned char *d;

	d = (unsigned char*)dest;
	if (d == s || n == 0)
		return (dest);
	if (d < s)
		while (n--)
			*d++ = *s++;
	else
	{
		d += n;
		s += n;
		while (n--)
			*--d = *--s;
	}
	return (dest);
}

int	ft_strclen(const char *str, int c)
{
	int i;

	i = 0;
	if (!str)
		return (0);
	while (str[i] && str[i] != c)
		i++;
	return (i);
}

char *ft_strchr(const char *str, int c)
{
	if (!str)
		return (NULL);
	while (*str)
	{
		if (*str == (char)c)
			return ((char *)str);
		str++;
	}
	return (NULL);
}

char *strndup(const char *str, int n)
{
	char *dup;

	if (!str)
		return (NULL);
	dup = malloc(sizeof(char) * (n + 1));
	if (!dup)
		return(NULL);
	ft_memmove(dup, str, n);
	dup[n] = '\0';
	return (dup);
}

char *strjoin(const char *src1, const char *src2)
{
	char	*join;
	int		len1;
	int		len2;

	if (!src1 || !src2)
		return (NULL);
	len1 = strclen(src1, '\0');
	len2 = strclen(src2, '\0');
	join = malloc(sizeof(char) * (len1 + len2 + 1));
	if (!join)
		return (NULL);
	ft_memmove(join, src1, len1);
	ft_memmove(join + len1, src2, len2);
	join[len1 + len2] = '\0';
	return (join);
}