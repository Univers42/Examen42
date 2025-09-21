/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:59:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/22 00:41:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	ensure_cap(char **line, size_t *cap, size_t need)
{
    char	*tmp;
    size_t	new_cap;
    size_t	old_cap;
    
    if (*cap >= need)
        return (1);
    old_cap = *cap;
    if (*cap)
        new_cap = *cap;
    else
        new_cap = 64;
    while (new_cap < need)
        new_cap *= 2;
    tmp = (char *)ft_realloc(*line, old_cap, new_cap);
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
    nl = (char *)ft_memchr(f->cur, '\n', avail);
    if (nl)
        chunk = (size_t)(nl - f->cur + 1);
    else
        chunk = avail;
    if (!ensure_cap(&line->buf, &line->cap, line->size + chunk + 1))
        return (-1);
    ft_memmove(line->buf + line->size, f->cur, chunk);
    line->size += chunk;
    line->buf[line->size] = '\0';
    f->cur += chunk;
    return (nl != NULL);
}

static int	gnl_refill(t_file *f, int fd)
{
    ssize_t	readn;

    readn = read(fd, f->buffer, BUFFER_SIZE);
    if (readn <= 0)
        return ((int)readn);
    f->cur = f->buffer;
    f->end = f->buffer + readn;
    return (1);
}

static int	scan_nl(t_file *gnl, t_dynstr *line, int fd)
{
    int	st;

    while (1)
    {
        if (gnl->cur >= gnl->end)
        {
            st = gnl_refill(gnl, fd);
            if (st <= 0)
                break ;
        }
        st = append_from_buffer(gnl, line);
        if (st == -1)
        {
            free(line->buf);
            line->buf = NULL;
            return (0);
        }
        if (st == 1)
            break ;
    }
    return (1);
}

char    *get_next_line(int fd)
{
    static t_file	gnl = {{0}, 0, 0};
    t_dynstr		line = {NULL, 0, 0};

    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    if (!scan_nl(&gnl, &line, fd))
        return (NULL);
    if (line.size == 0)
    {
        free(line.buf);
        return (NULL);
    }
    return (line.buf);
}