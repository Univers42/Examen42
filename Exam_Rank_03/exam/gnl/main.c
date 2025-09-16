/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:23:06 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/16 18:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

# define BUFFER_SIZE 1024

char	*store_chunks(int fd, char *memory)
{
	char	*buffer;
	char	*new;
	ssize_t	read_bytes;
	bool	found_newline;
	int		size;

	read_bytes = 1;
	buffer = malloc(sizeof(char) * BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	found_newline = strclen(memory, '\n', read_bytes);	
	while (!found_newline)
	{
		read_bytes = read(fd, buffer, BUFFER_SIZE);
		if (read_bytes < 0)
			return (free(buffer), memory);
		if (read_bytes == 0)
			break;
		buffer[read_bytes] = '\0';
		new = ft_strjoin(memory, buffer);
		if (!new)
			return (free(buffer), NULL);
		memory = new;
	}
	return (free(buffer), memory);
}

static char	*ft_get_line(char *memory)
{
	char	*line;
	char	*ptr;

	if (!memory || !*memory)
		return (NULL);
	ptr = memory;
	while (*ptr && *ptr != '\n')
		ptr++;
	line = malloc((size_t)(ptr - memory + 2));
	if (!line)
		return (NULL);
	ft_memmove(line, memory, (size_t)(ptr - memory + 1));
	line[ptr - memory + 1] = '\0';
	return (line);
}

static char	*ft_get_leftover(char *memory)
{
	char	*ptr;
	char	*leftover;
	int		size;

	ptr = memory;
	strclen(ptr, '\0', &size);
	while (*ptr && *ptr != '\n')
		ptr++;
	if (!*ptr)
		return (free(memory), NULL);
	leftover = ft_strndup(ptr + 1, (size_t)size);
	return (free(memory), leftover);
}

char *get_next_line(int fd)
{
	static char	*memory = NULL;
	char		*line;
	
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	memory = store_chunks(memory, line);
	if (!memory)
		return (NULL);
	line = get_line(memory);
	memory = get_leftover(memory);
	return (line);
}

int main(int argc, char **argv)
{
	/**
	 * TEST LABOPHASE
	 */
	
	
	/**
	 * TEST GET_NEXT_LINE
	 * 
	 */
	printf("test labo");
	
}