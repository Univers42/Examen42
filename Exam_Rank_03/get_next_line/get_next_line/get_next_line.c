/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 23:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/15 11:04:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"

#include <signal.h>

static volatile sig_atomic_t g_stop = 0;

static char *store_chunks(int fd, char *memory)
{
	char *buffer;
	char *new;
	ssize_t bytes_read;
	int size;

	buffer = malloc(sizeof(char) * BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	while (1)
	{
		if (g_stop)
			break;
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			break; // interrupted or error; exit loop to free buffer
		if (bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';
		if (memory)
		{
			char *old = memory;
			new = ft_strjoin(memory, buffer);
			if (!new)
				return (free(buffer), NULL);
			memory = new;
			free(old);
		}
		else
		{
			new = ft_strndup(buffer, (int)bytes_read);
			if (!new)
				return (free(buffer), NULL);
			memory = new;
		}
		if (strclen(buffer, '\n', &size))
			break;
	}
	return (free(buffer), memory);
}

static char *get_line(char *memory)
{
	char *line;
	char *ptr;

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

static char *get_leftover(char *memory)
{
	char *ptr;
	char *leftover;
	int size;

	ptr = memory;
	while (*ptr && *ptr != '\n')
		ptr++;
	if (!*ptr)
		return (free(memory), NULL);
	strclen(ptr + 1, '\0', &size);
	leftover = ft_strndup(ptr + 1, size);
	return (free(memory), leftover);
}

static char *get_gnl_static_memory(int set, char *new_mem)
{
	static char *memory = NULL;

	if (set)
		memory = new_mem;
	return (memory);
}

char *get_next_line(int fd)
{
	char *line;
	char *memory = get_gnl_static_memory(0, NULL);

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	memory = store_chunks(fd, memory);
	if (!memory)
		return (NULL);
	line = get_line(memory);
	memory = get_leftover(memory);
	get_gnl_static_memory(1, memory);
	return (line);
}

void free_gnl_memory(void)
{
	char *memory = get_gnl_static_memory(0, NULL);

	if (memory)
		free(memory);
	get_gnl_static_memory(1, NULL);
}

static void handle_sigint(int sig)
{
	(void)sig;
	g_stop = 1;
}

int main(int argc, char **argv)
{
	int fd;
	char *line;

	signal(SIGINT, handle_sigint);
	if (argc == 2)
		fd = open(argv[1], O_RDONLY);
	else
		fd = 0;
	if (fd < 0)
	{
		perror("open");
		return 1;
	}
	while (!g_stop && (line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
	}
	free_gnl_memory();
	if (argc == 2)
		close(fd);
	return 0;
}