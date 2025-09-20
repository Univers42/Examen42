/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:52:08 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/16 22:50:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 100000
#endif

int		ft_strncmp(const char *src1, const char *src2, size_t n)
{
	while (n--)
	{
		if (*src1 != *src2 || !*src1)
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
	char	c;
	ssize_t	read_b;
	char	*buffer;
	size_t	total_read = 0;
	size_t	capacity = BUFFER_SIZE;
	char	*tmp;

	if (argc != 2)
		return (printf("[ERROR].%s <pattern>", argv[0]), 1);
	c = '*';
	pattern = argv[1];
	buffer = malloc(capacity);
	if (!buffer)
		return (1);
	read_b = read(0, buffer + total_read, BUFFER_SIZE);
	while (/**check if bytes are read or not */)
	{
		/**
		 * to implement the reallocation
		 * we need to verify if the current
		 * buffer size is already higher than the current capacity
		 * if it's the case then we need to realloc incremen doubing the capacity
		 * 
		 * 
		 */
		total_read += read_b;
		if (total_read + BUFFER_SIZE > capacity)
		{
			capacity += BUFFER_SIZE;
			tmp = realloc(buffer, capacity);
			if (!tmp)
				return (free(buffer),1);
			buffer = tmp;
		}
		//we increment reading again and again
	}
	if (total_read == 0)
		return (free(buffer), 1);
	if (total_read > 0 && buffer[total_read - 1] == '\n')
		buffer[total_read - 1] = '\0';
	else
		buffer[total_read] = '\0';
	filter(buffer, pattern, c);
	printf("\nresult = %s\n", buffer);
	return (0);
}


int main(int argc, char **argv)
{
	size_t	total_read;
	char	*map;
	char	*map;
}
