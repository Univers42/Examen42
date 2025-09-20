/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:54:06 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/19 20:41:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	read_more(t_file *f, int fd)
{
	ssize_t	bytes;

	compact_if_needed(f);
	if (!gnl_grow(f, (size_t)BUFFER_SIZE + 1))
		return (0);
	bytes = read(fd, f->end, BUFFER_SIZE);
	if (bytes <= 0)
	{
		if (bytes < 0)
			GNL_SET(f, GNL_F_ERR);
		else
			GNL_SET(f, GNL_F_EOF);
		return (0);
	}
	f->end += (size_t)bytes;
	return (1);
}

static char	*scan_loop(t_file *f, int fd)
{
	char	*line;

	while (1)
	{
		line = scan_buffer(f);
		if (line)
			return (line);
		if (!read_more(f, fd))
		{
			if (GNL_HAS(f, GNL_F_ERR))
				return (NULL);
			return (drain_tail_or_null(f));
		}
	}
}

char	*get_next_line(int fd)
{
	t_file	*f;

	f = gnl_state();
	if (fd == -1)
	{
		gnl_reset(f);
		return (NULL);
	}
	if (BUFFER_SIZE <= 0 || fd < 0)
		return (NULL);
	return (scan_loop(f, fd));
}
