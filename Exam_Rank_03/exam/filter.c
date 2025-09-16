/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:52:08 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/16 19:17:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

# define BUFFER_SIZE 100000

int		ft_strncmp(const char *src1, const char *src2, size_t n)
{
	while (n--)
	{
		if (*src1 != *src2 && !*src1)
			return((unsigned char)*src1 - (unsigned char)*src2);
		src1++;
		src2++;
	}
	return (0);
}

void	filter(char *haystack, char *needle, int c)
{
	char	*writer;
	int		count;
	int		needle_len;

	needle_len = strlen(needle);
	while(*haystack)
	{
		if (ft_strncmp(haystack, needle, needle_len) == 0)
		{
			writer = haystack;
			count = needle_len;
			while (count--)
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
	char		c;
	ssize_t	read_b;
	char	buffer[BUFFER_SIZE];

	if (argc != 2)
		return (printf("[ERROR].%s <pattern>", argv[0]), 1);
	c = '*';
	pattern = argv[1];
	read_b = read(0, buffer, BUFFER_SIZE - 1);
	if (read_b <= 0)
		return (printf("[ERROR], couldn't read the stdin"), 1);
	if (read_b > 0 && read_b < BUFFER_SIZE && buffer[read_b - 1] == '\n')
		buffer[read_b - 1] = '\0';
	buffer[read_b] = '\0';
	filter(buffer, pattern, c);
	printf("\nresult = %s\n", buffer);
	return (0);
}
