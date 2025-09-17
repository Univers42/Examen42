/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 11:31:16 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/17 14:37:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

# define NL '\n'

typedef struct s_gnl_state {
	char   *buf;    // base
	char   *start;  // first unread
	char   *end;    // one past last valid
	size_t cap;     // total capacity
} t_gnl_state;

static int ensure_capacity(t_gnl_state *st)
{
	size_t	used;
	size_t	new_cap;
	char	*old_base;
	char	*tmp;

	if (st->buf && (size_t)(st->cap - (st->end - st->buf)) > (size_t)BUFFER_SIZE)
		return 1;
	used = st->buf ? (size_t)(st->end - st->buf) : 0;
	new_cap = st->cap ? st->cap * 2 : (size_t)BUFFER_SIZE * 2;
	if (new_cap < used + (size_t)BUFFER_SIZE + 1)
		new_cap = used + (size_t)BUFFER_SIZE + 1;
	old_base = st->buf;
	tmp = ft_realloc(st->buf, st->cap, new_cap);
	if (!tmp)
		return 0;
	st->buf = tmp;
	st->cap = new_cap;
	if (!old_base)
		st->start = st->end = st->buf;
	else
	{
		//rebase pointer after realloc
		st->start = st->buf + (st->start - old_base);
		st->end   = st->buf + (st->end   - old_base);
	}
	return (1);
}

static void compact_if_needed(t_gnl_state *st)
{
	size_t	free_tail;
	size_t	unread;

	if (st->buf && st->start != st->buf)
	{
		free_tail = st->cap - (size_t)(st->end - st->buf);
		if (free_tail <= (size_t)BUFFER_SIZE && st->end > st->start)
		{
			unread = (size_t)(st->end - st->start);
			ft_memmove(st->buf, st->start, unread);
			st->start = st->buf;
			st->end = st->buf + unread;
		}
	}
}

static char *emit_line(t_gnl_state *st, char *nl_ptr)
{
	size_t	len;
	char	*line;

	len = (size_t)(nl_ptr - st->start) + 1;
	line = malloc(len + 1);
	if (!line)
		return NULL;
	ft_memmove(line, st->start, len);
	line[len] = '\0';
	st->start += len;
	if (st->start == st->end)
		st->start = st->end = st->buf;
	return (line);
}

static char *emit_tail(t_gnl_state *st)
{
	size_t	len;
	char	*line;

	if (!st->buf || st->start == st->end)
		return NULL;
	len = (size_t)(st->end - st->start);
	line = malloc(len + 1);
	if (!line)
		return NULL;
	ft_memmove(line, st->start, len);
	line[len] = '\0';
	free(st->buf);
	st->buf = NULL;
	st->start = st->end = NULL;
	st->cap = 0;
	return (line);
}

void	cleanup_static(t_gnl_state st)
{
	if (st.buf)
	{
		free(st.buf);
		st.buf = NULL;
		st.start = st.end = NULL;
		st.cap = 0;	
	}
}

char *get_next_line(int fd)
{
	static t_gnl_state	st = {NULL, NULL, NULL, 0};
	char				*p;
	ssize_t				r;

	// cleanup request
	if (fd == -1)
		return (cleanup_static(st), NULL);
	if (BUFFER_SIZE <= 0 || fd < 0)
		return NULL;
	while (1)
	{
		if (st.buf)
		{
			p = st.start;
			while (p < st.end && *p != NL)
				p++;
			if (p < st.end && *p == NL)
				return emit_line(&st, p);
		}
		compact_if_needed(&st);
		if (!ensure_capacity(&st))
			return NULL;
		r = read(fd, st.end, BUFFER_SIZE);
		if (r <= 0)
			return (r < 0) ? NULL : emit_tail(&st);
		st.end += (size_t)r;
		*st.end = '\0';
	}
}

int main(int argc, char **argv)
{
	int		fd;
	char	*line;

	if (argc != 2)
	{
		printf("usage: %s <file>\n", argv[0]);
		return (1);
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return(perror("open"), 1);
	while ((line = get_next_line(fd)))
	{
		printf("%s", line);
		if (line[0] && line[strlen(line)-1] != '\n')
			printf("\n"); free(line); }
	close(fd);
	get_next_line(-1);
	return 0;
}
