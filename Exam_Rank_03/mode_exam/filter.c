/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 15:25:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/15 21:07:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define BUFFER_SIZE 100000

int	ft_strncmp(const char *s1, const char *s2, int n)
{
	while (n-- > 0)
	{
		if (*s1 != *s2 || *s1 == '\0')
			return ((unsigned char)*s1 - (unsigned char)*s2);
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

	needle_len = strlen(needle);
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
	char	*pattern;
	char	buffer[BUFFER_SIZE];
	ssize_t	read_bytes;
	char	*needle;

	if (argc != 2 || argv[1][0] == '\0' || argv[1] == NULL)
		return (printf("%s <pattern>\n", argv[0]));
	pattern = argv[1];
	read_bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
	if (read_bytes <= 0)
	{
		printf("%s", "[ERROR] couldn't read from stdin or empty\n");
		return (1);
	}
	if (read_bytes > 0 && buffer[read_bytes - 1] == '\n')
		buffer[read_bytes - 1] = '\0';
	buffer[read_bytes] = '\0';
	//int test_res = ft_strncmp("helsdfs",  "heladsfd", 4);
	//printf("finally -> %s\n", test_res == 0 ? "equal" : "not equal");
	//printf("string read : %s\n buffer_read: %zd\n", buffer, read_bytes);
	filter(buffer, pattern, '*');
	printf("%s\n", buffer);
	return (0);
}