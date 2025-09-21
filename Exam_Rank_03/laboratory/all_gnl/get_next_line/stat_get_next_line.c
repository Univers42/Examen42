/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat_get_next_line.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 17:14:02 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/21 17:44:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif 

# define LINE_MAX 100000

typedef struct s_gnl
{
	char	buffer[BUFFER_SIZE];
	size_t	len; // number of valid bytes in buffer
	char	*slow; // current consumption pointer
	char	*fast; // scanning pointer
} t_gnl;

void	reset_gnl(t_gnl *gnl)
{
	if (gnl)
	{
		gnl->len = 0;
		gnl->slow = gnl->buffer;
		gnl->fast = gnl->buffer;
	}
}

// Replace previous find_nl with a refill helper and dynamic assembly of the line
static int	refill(t_gnl *file, int fd)
{
	size_t remaining = 0;
	if (file->slow && file->slow < file->buffer + file->len)
		remaining = (size_t)((file->buffer + file->len) - file->slow);
	if (remaining && file->slow != file->buffer)
		memmove(file->buffer, file->slow, remaining);
	file->len = remaining;
	file->slow = file->buffer;
	file->fast = file->buffer + remaining;
	ssize_t r = read(fd, file->buffer + remaining, BUFFER_SIZE - remaining);
	if (r <= 0)
		return (int)r; // 0 = EOF, -1 = error
	file->len += (size_t)r;
	file->slow = file->buffer;
	file->fast = file->buffer;
	return 1;
}

char	*get_next_line(int fd)
{
	static t_gnl	file = { {0}, 0, NULL, NULL };
	char			*line = NULL;
	size_t			size = 0, cap = 0;

	if (fd == -1)
		return (reset_gnl(&file), NULL);
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (file.slow == NULL || file.fast == NULL)
		reset_gnl(&file);
	while (1)
	{
		if (file.slow >= file.buffer + file.len)
		{
			int st = refill(&file, fd);
			if (st <= 0)
				break;
		}
		char *buf_end = file.buffer + file.len;
		if (file.fast < file.slow)
			file.fast = file.slow;
		size_t avail = (size_t)(buf_end - file.slow);
		char *nl = memchr(file.slow, '\n', avail);
		size_t chunk = nl ? (size_t)(nl - file.slow + 1) : avail;
		if (size + chunk + 1 > cap)
		{
			size_t new_cap = cap ? cap : 64;
			while (new_cap < size + chunk + 1)
				new_cap *= 2;
			char *tmp = realloc(line, new_cap);
			if (!tmp)
			{
				free(line);
				return NULL;
			}
			line = tmp;
			cap = new_cap;
		}
		memmove(line + size, file.slow, chunk);
		size += chunk;
		line[size] = '\0';
		file.slow += chunk;
		file.fast = file.slow;
		if (nl)
			break;
	}
	if (size == 0)
	{
		free(line);
		return NULL;
	}
	return line;
}

__attribute__((weak))
int main(int argc, char **argv)
{
	char	*line;
	int fd = open(argv[1], O_RDWR);

	// printf("%s", get_next_line(O_RDWR));
	// printf("%s", get_next_line(fd));
	// printf("%s", get_next_line(fd));
	// printf("%s", get_next_line(fd));
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}
