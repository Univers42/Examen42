/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:52:29 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 14:24:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

#ifndef GLYPH
# define GLYPH '*'
#endif

int ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *c1 = (const unsigned char*)s1;
	const unsigned char *c2 = (const unsigned char*)s2;

	while (n--)
	{
		if (*c1 != *c2)
			return (*c1 - *c2);
		c1++;
		c2++;
	}
	return (0);
}

void	filter(char *haystack, const char *needle, int c)
{
	const int		needle_len = strlen(needle);
	char			*writer;
	int				count;

	while (*haystack)
	{
		if (ft_memcmp(haystack, needle, needle_len) == 0)
		{
			writer = haystack;
			count = needle_len;
			while (count-- > 0)
				*writer++ = (char)c;
			haystack += needle_len;
		}
		else
			haystack++;
	}

}

int main(int argc, char **argv)
{
	char	*pattern;
	char	*buffer;
	ssize_t	rbytes;
	size_t	capacity;
	size_t	total_read;
	char	*tmp;
	int		c;

	if (argc != 2)
		return (1);
	c = GLYPH;
	capacity = BUFFER_SIZE;
	buffer = malloc(sizeof(char) * capacity);
	rbytes = read(STDIN_FILENO, buffer, capacity);
	pattern = argv[1];
	total_read = 0;
	while (rbytes > 0)
	{
		total_read += rbytes;
		if (total_read >= capacity)
		{
			capacity *= 2;
			tmp = realloc(buffer, capacity);
			if (!tmp)
				return (free(pattern), 1);
			buffer = tmp;
		}
		rbytes = read(STDIN_FILENO, buffer + total_read, capacity - total_read);
	}
	buffer[total_read] = '\0';	// if not done, then error with valgrind
	filter(buffer, pattern, c);
	printf("%s", buffer);
	free(buffer);
	return (0);
}