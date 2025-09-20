/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 23:22:53 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/15 10:52:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"

bool strclen(const char *str, int c, int *size)
{
	const char *cpy = str;

	while (*cpy && *cpy != (char)c)
		cpy++;
	*size = cpy - str;
	return (*cpy == (char)c);
}
char *ft_strndup(const char *str, int n)
{
	char *dup;
	char *cpy;
	int size;
	int len;

	strclen(str, '\0', &size);
	len = (n < size) ? n : size;
	dup = malloc(sizeof(char) * (len + 1));
	if (!dup)
		return (NULL);
	cpy = dup;
	while (*str && len--)
		*dup++ = *str++;
	*dup = '\0';
	return (cpy);
}

void *ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char *d;
	const unsigned char *s;

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	if (d == s)
		return (dst);
	if (d < s)
		while (len--)
			*d++ = *s++;
	else
	{
		d += len;
		s += len;
		while (len--)
			*--d = *--s;
	}
	return (dst);
}

char *ft_strjoin(const char *s1, const char *s2)
{
	int len_s1;
	int len_s2;
	char *joined;
	char *ptr;

	strclen(s1, '\0', &len_s1);
	strclen(s2, '\0', &len_s2);
	joined = malloc(sizeof(char) * (size_t)(len_s1 + len_s2 + 1));
	if (!joined)
		return (NULL);
	ptr = joined;
	while (len_s1--)
		*ptr++ = *s1++;
	while (len_s2--)
		*ptr++ = *s2++;
	*ptr = '\0';
	return (joined);
}

__attribute__((weak))
int main(void)
{
	char *s = "Hello, world!";
	char *dup;

	dup = ft_strndup(s, 5);
	printf("ft_strndup(s, 5): '%s'\n", dup);
	free(dup);

	dup = ft_strndup(s, 20);
	printf("ft_strndup(s, 20): '%s'\n", dup);
	free(dup);

	return 0;
}
