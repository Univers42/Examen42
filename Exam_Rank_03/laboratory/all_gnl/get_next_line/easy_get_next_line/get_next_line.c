/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 17:14:02 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/21 17:56:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <sys/types.h>

typedef struct s_gnl
{
	char	buffer[BUFFER_SIZE];
	char	*cur;
	char	*end;
}t_gnl;

static int	ensure_cap(char **line, size_t *cap, size_t need)
{
	char	*tmp;
	size_t	new_cap;

	if (*cap >= need)
		return (1);
	new_cap = *cap ? *cap : 64;
	while (new_cap < need)
		new_cap *= 2;
	tmp = (char *)realloc(*line, new_cap);
	if (!tmp)
		return (0);
	*line = tmp;
	*cap = new_cap;
	return (1);
}

static int	append_from_buffer(
				t_gnl *gnl, char **line, size_t *size, size_t *cap)
{
	char	*nl;
	size_t	avail;
	size_t	chunk;

	if (gnl->cur >= gnl->end)
		return (0);
	avail = (size_t)(gnl->end - gnl->cur);
	nl = (char *)memchr(gnl->cur, '\n', avail);
	if (nl)
		chunk = (size_t)(nl - gnl->cur + 1);
	else
		chunk = avail;
	if (!ensure_cap(line, cap, *size + chunk + 1))
		return (-1);
	memmove(*line + *size, gnl->cur, chunk);
	*size += chunk;
	(*line)[*size] = '\0';
	gnl->cur += chunk;
	return (nl != NULL);
}

void	reset_gnl(t_gnl *gnl)
{
	if (!gnl)
		return ;
	gnl->cur = gnl->buffer;
	gnl->end = gnl->buffer;
}

static int	gnl_init_if_needed(t_gnl *gnl)
{
	if (gnl->cur == NULL || gnl->end == NULL)
		reset_gnl(gnl);
	return (1);
}

static int	gnl_refill(t_gnl *gnl, int fd)
{
	ssize_t	readn;

	readn = read(fd, gnl->buffer, BUFFER_SIZE);
	if (readn <= 0)
		return ((int)readn);
	gnl->cur = gnl->buffer;
	gnl->end = gnl->buffer + readn;
	return (1);
}

char	*get_next_line(int fd)
{
	static t_gnl	gnl = {{0}, NULL, NULL};
	char			*line;
	size_t			size;
	size_t			cap;
	int				st;

	line = NULL;
	size = 0;
	cap = 0;
	if (fd == -1)
		return (reset_gnl(&gnl), NULL);
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	gnl_init_if_needed(&gnl);
	while (1)
	{
		if (gnl.cur >= gnl.end)
		{
			st = gnl_refill(&gnl, fd);
			if (st <= 0)
				break ;
		}
		st = append_from_buffer(&gnl, &line, &size, &cap);
		if (st == -1)
			return (free(line), NULL);
		if (st == 1)
			break ;
	}
	if (size == 0)
		return (free(line), NULL);
	return (line);
}

__attribute__((weak))
int	main(int argc, char **argv)
{
	char	*line;
	int		fd;

	(void)argc;
	fd = open(argv[1], O_RDWR);
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}
