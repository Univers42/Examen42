
#include "get_next_line.h"

static int	ensure_cap(char	**line, size_t *cap, size_t need)
{
	void	*tmp;
	size_t	new_cap;

	if (*cap >= need)
		return (1);
	if (*cap)
		new_cap = *cap;
	else
		new_cap = 64;
	while (new_cap < need)
		new_cap *= 2;
	tmp = ft_realloc(*line, *cap, new_cap);
	if (!tmp)
		return (0);
	*line = tmp;
	*cap = new_cap;
	return (1);
}

static int	refill(t_file *stream, int fd)
{
	ssize_t	readn;

	readn = read(fd, stream->buf, BUFFER_SIZE);
	if (readn <= 0)
		return ((int)readn);
	stream->cur = stream->buf;
	stream->end = stream->buf + readn;
	return (1);
}

static int	append_from_buffer(t_file *stream, t_dynstr *line)
{
	char	*nl;
	size_t	avail;
	size_t	chunk;

	avail = (size_t)(stream->end - stream->cur);
	nl = (char *)ft_memchr(stream->cur, '\n', avail);
	if (nl)
		chunk = (size_t)(nl - stream->cur + 1);
	else
		chunk = avail;
	if (!ensure_cap(&line->buf, &line->cap, line->size + chunk + 1))
		return (-1);
	ft_memmove(line->buf + line->size, stream->cur, chunk);
	line->size += chunk;
	line->buf[line->size] = '\0';
	stream->cur += chunk;
	return (nl != NULL);
}

static int		scan_nl(t_file *stream, t_dynstr *line, int fd)
{
	int st;

	while (1)
	{
		if (stream->cur >= stream->end)	// error > instead of >=
		{
			st = refill(stream, fd);
			if (st <= 0)
				break ;
		}
		st = append_from_buffer(stream, line);
		if (st == -1)
			return (0);
		if (st == 1)
			break ;
	}
	return (1);
}

char	*get_next_line(int fd)
{
	static t_file	stream = {{0}, 0, 0};
	t_dynstr		line = {NULL,0,0};

	if (fd < 0 || BUFFER_SIZE <= 0)
		return  (NULL);
	if (!scan_nl(&stream, &line, fd))
		return (RESET_BUF(line.buf), NULL);
	if (line.size == 0)
		return (RESET_BUF(line.buf), NULL);
	return (line.buf);
}

__attribute__((weak))
int	main(int argc, char **argv)
{
	char	*line;
	int		fd;

	if (argc != 2)
		return (1);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (2);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	return (0);
}
