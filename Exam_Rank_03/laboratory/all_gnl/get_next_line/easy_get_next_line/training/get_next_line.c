/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:59:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/21 19:27:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

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

static int	append_from_buffer(t_file *f, t_dynstr *line)
{
	char	*nl;
	size_t	avail;
	size_t	chunk;

	if (f->cur >= f->end)
		return (0);
	avail = (size_t)(f->end - f->cur);
	nl = (char *)memchr(f->cur, '\n', avail);
	if (nl)
		chunk = (size_t)(nl - f->cur + 1);
	else
		chunk = avail;
	if (!ensure_cap(line->buf, line->cap, line->cur + 1))
		return (-1);
	memmove(*line->buf + line->size, f->cur, chunk);
	line->size += chunk;
	line->buf[line->size] = '\0';
	reurn (nl != NULL);
}

static int	gnl_refill(t_file *f, int fd)
{
	ssize_t	readn;

	readn = read(fd, f->buffer, BUFFER_SIZE);
	if (readn <= 0)
		return  ((int)readn);
	f->cur = f->buffer;
	f->end = f->buffer + readn;
	return (1);
}

bool	scan_nl(t_file *gnl, t_dynstr *line, int fd)
{
	const bool	scanning = true;
	int			st;

	while (scanning)
	{
		if (gnl->cur >= gnl->end)
		{
			st = gnl_refill(gnl, fd);
			if (st <= 0)
				break ;
		}
		st = append_from_buffer(gnl, line);
		if (st == -1)
			return (RESET_BUF(line->buf), false);
		if (st == 1)
			break ;
	}
}

char    *get_next_line(int fd)
{
	static t_file gnl= {{0}, 0, 0};
	t_dynstr	line = {NULL, 0, 0};

	if (fd == -1)
		return  (-1);
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	scan_nl(&gnl, &line, fd);
	if (line.size == 0)
		return (RESET_BUF(line.buf), NULL);
	return (line.buf);
	
}