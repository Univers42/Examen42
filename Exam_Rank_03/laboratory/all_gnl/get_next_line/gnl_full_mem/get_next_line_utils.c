/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:20:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/19 20:42:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"


void	*ft_memmove(void *dst, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;

	if (dst == src || n == 0)
		return (dst);
	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	if (d < s)
	{
		while (n > 0)
		{
			*d = *s;
			d++;
			s++;
			n--;
		}
	}
	else
	{
		d += n;
		s += n;
		while (n > 0)
		{
			d--;
			s--;
			*d = *s;
			n--;
		}
	}
	return (dst);
}

void	compact_if_needed(t_file *f)
{
	size_t tail_free;
	size_t unread;

	if (!f || !f->base || f->start == f->base)
		return ;
	tail_free = GNL_TAIL_FREE(f);
	if (tail_free <= (size_t)BUFFER_SIZE && f->end > f->start)
	{
		unread = GNL_UNREAD(f);
		ft_memmove(f->base, f->start, unread);
		f->start = f->base;
		f->end = f->base + unread;
	}
}

static char	*emit_span(t_file *f, char *span_end, int include_nl)
{
	char	*real_end;
	size_t	len;
	char	*out;

	real_end = span_end;
	if (include_nl)
		real_end = span_end + 1;
	len = (size_t)(real_end - f->start);
	out = (char *)malloc(len + 1);
	if (!out)
		return (NULL);
	ft_memmove(out, f->start, len);
	out[len] = '\0';
	f->start = real_end;
	if (f->start == f->end)
	{
		f->start = f->base;
		f->end = f->base;
	}
	return (out);
}

char	*drain_tail_or_null(t_file *f)
{
	char	*tail;

	if (!f || !f->base || f->start == f->end)
		return (NULL);
	tail = emit_span(f, f->end, 0);
	if (tail && f->start == f->end)
		gnl_reset(f);
	return (tail);
}

char	*scan_buffer(t_file *f)
{
	char	*p;

	if (!f || !f->base)
		return (NULL);
	p = f->start;
	while (p < f->end && *p != NL)
		p++;
	if (p < f->end && *p == NL)
		return (emit_span(f, p, 1));
	return (NULL);
}

static t_file	*g_state(void)
{
	static t_file s = {0, NULL, NULL, NULL, 0};
	return (&s);
}

t_file	*gnl_state(void)
{
	return (g_state());
}

void	gnl_reset(t_file *f)
{
	if (!f)
		return ;
	if (f->base)
		free(f->base);
	f->base = NULL;
	f->start = NULL;
	f->end = NULL;
	f->cap = 0;
	f->flags = GNL_F_NONE;
}

void	gnl_cleanup(void)
{
	t_file	*f;

	f = g_state();
	gnl_reset(f);
}

static size_t	cap_next(size_t current, size_t used, size_t need)
{
	size_t new_cap;

	if (current > 0)
		new_cap = current * 2;
	else
		new_cap = GNL_INIT_CAP;
	if (new_cap < used + need)
		new_cap = used + need;
	return (new_cap);
}

int	gnl_grow(t_file *f, size_t need)
{
	size_t	used;
	size_t	avail;
	size_t	new_cap;
	char	*old;
	char	*p;

	if (!f)
		return (0);
	if (f->base)
	{
		used = GNL_USED(f);
		avail = f->cap - used;
		if (avail >= need)
			return (1);
	}
	else
		used = 0;
	new_cap = cap_next(f->cap, used, need);
	old = f->base;
	p = (char *)malloc(new_cap);
	if (!p)
		return (0);
	if (old)
	{
		ft_memmove(p, old, GNL_USED(f));
		f->start = p + (f->start - old);
		f->end = p + (f->end - old);
		free(old);
	}
	else
	{
		f->start = p;
		f->end = p;
	}
	f->base = p;
	f->cap = new_cap;
	return (1);
}
