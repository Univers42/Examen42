/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:23:06 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/16 20:56:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"
#include <fcntl.h>
#include <stdio.h>

# define BUFFER_SIZE 1024

char	*get_leftover(char *memory)
{
	char	*leftover;
	size_t	jump;
	size_t	len;

	len = ft_strclen(memory, '\0');
	jump = ft_strclen(memory, '\n');
	if (memory[jump] == '\n')
		jump++;
	leftover = ft_strndup(memory + jump, len - jump + 1);
	if (!leftover)
		return (NULL);
	free(memory);
	return (leftover);
}

char	*get_line(char *memory)
{
	char	*line;
	size_t	len;

	len = ft_strclen(memory, '\n');
	if (memory[len] == '\n')
		len++;
	line = ft_strndup(memory, len);
	if (!line)
		return (NULL);
	return (line);
}

char	*store_chunks(int fd, char *memory)
{
	char	*chunk;
	ssize_t	bytes;

	bytes = 1;
	chunk = (char *)malloc(BUFFER_SIZE + 1);
	if (!chunk)
		return (NULL);
	while (bytes > 0 && !ft_strchr(memory, '\n'))
	{
		bytes = read(fd, chunk, BUFFER_SIZE);
		if (bytes == 0)
			break ;
		if (bytes == -1)
		{
			free(chunk);
			return (NULL);
		}
		chunk[bytes] = '\0';
		memory = ft_strjoin(memory, chunk);
	}
	free(chunk);
	if (ft_strclen(memory, '\0') > 0)
		return (memory);
	return (NULL);
}

char	*get_next_line(int fd)
{
	static char	*memory;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	memory = store_chunks(fd, memory);
	if (!memory)
		return (NULL);
	line = get_line(memory);
	memory = get_leftover(memory);
	if (!memory[0])
	{
		free(memory);
		memory = NULL;
	}
	return (line);
}

int main(void)
{
	char	*line;
	int		fd;

	fd = open("file.txt", O_RDONLY);
	line = get_next_line(fd);
	while(line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
}
