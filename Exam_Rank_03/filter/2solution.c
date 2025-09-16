/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2solution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 20:02:28 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/15 20:02:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

#define BUFFER_MAX 100000

int	ft_strncmp(const char *s1, const char *s2, int n)
{
	while (n-- > 0)
	{
		if (*s1 != *s2 || *s1 == '\0')
			return (*(unsigned char *)s1 - *(unsigned char *)s2);
		s1++;
		s2++;
	}
	return (0);
}

void filter(char *haystack, char *needle, char to_replace)
{
	int		needle_len;
	char	*writer;
	int		count;

	strlen(needle);
	while (*haystack)
	{
		if (ft_strncmp(haystack, needle, needle_len) == 0)
		{
			writer = haystack;
			count = needle_len;
			while (count-- > 0)
				*writer++ = to_replace;
			haystack += needle_len;
		}
		else
			haystack++;
	}
}

int main(int argc, char **argv)
{
	char *needle;
	char buffer[BUFFER_MAX];
	ssize_t bytes_read;
	char *pattern;

	if (argc != 2 || argv[1][0] == '\0' || argv[1] == NULL)
		return (printf("%s <pattern>\n", argv[0]));
	bytes_read = read(STDIN_FILENO, buffer, BUFFER_MAX - 1);
	if (bytes_read <= 0)
	{
		printf("%s", "[ERROR] couldn't read from stdin or empty\n");
		return (1);
	}
	if (bytes_read > 0 && buffer[bytes_read - 1] == '\n')
		buffer[bytes_read - 1] = '\0';
	buffer[bytes_read] = '\0';
	pattern = argv[1];
	filter(buffer, pattern, '*');
	printf("%s\n", buffer);
	return (0);
}