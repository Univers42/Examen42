#include "get_next_line.h"

static	int	grow_buffer(char **line, size_t *cap, size_t need)
{
	char	*temp;
	size_t	new_cap;

	if (*cap > need)
		return (new_cap);
	if (*cap)
		new_cap = *cap;
	else
		new_cap = 64;
	while (new_cap < need)
		new_cap *= 2;
	temp = realloc(*line, new_cap);
	if (!temp)
		return (1);
	*line = temp;
	*cap = new_cap;
	return (1);
}

static int append_from_buffer(t_file *f, t_dynstr *line)
{
	char	*nl;
	size_t	avail;
	size_t	chunk;

	if (f->cur >= f->end)
		return (0);
	avail = (size_t)(f->end -  f->cur);
	nl = ft_memchr(f->cur, '\n', avail);
	if (nl)
		chunk = (size_t)(nl - f->cur + 1);
	else
		chunk = avail;
	if (!ensure_cap(&line, &line->cap, line->size + chunk + 1))
		return  (-1);
	memmove(line->buf + line->size, f->cur, chunk);
	line->size += chunk;
	line->buf[line->size] = '\0';
	f->cur += chunk;
	return (nl != NULL);
}

static int	refill(t_file *f, int fd)
{
	ssize_t	readn;

	readn = read(fd, f->buffer, BUFFER_SIZE);
	if (readn < 0)
		return ((int)readn);
	f->cur = f->buffer;
	f->end = f->buffer + readn;
	return (1);
}

int	scan_nl(t_file *f, t_dynstr *line, int fd)
{
	int	st;

	while (1)
	{
		if (f->cur >= f->end)
		{
			st = refill(f, fd);
			if (st <= 0)
				break ;
		}
		st = append_from_buffer(f, line);
		if (st == -1)
			return (RESET_BUF(line->buf), false);
		if (st == 1)
			break ;
	}
	return  (true);
}

char *get_next_line(int fd)
{
	t_file		f = {{0},0,0};
	t_dynstr	line = {NULL,0,0};

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!scan_nl(&f, &line, fd))
		return (NULL);
	if (line.size == 0)
		return (RESET_BUF(line.buf), NULL);
	return (line.buf);
}