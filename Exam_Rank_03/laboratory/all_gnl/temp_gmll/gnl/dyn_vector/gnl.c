#include <stdlib.h>
#include <stdint.h>
#include "get_next_line.h"

#define FD_MAX 1024

bool	append_from_buf_to_line(t_buf *buf, t_dynstr *line)
{
	size_t	begin;

	begin = buf->index;
	while (buf->index < (size_t)buf->bytes_read && buf->buf[buf->index] != '\n')
		++buf->index;
	if (buf->index < (size_t) buf->bytes_read && buf->buf[buf->index] == '\n')
	{
		buf->nl_found = true;
		++buf->index;
	}
	if (!dynstr_ensure_size(line, line->len + (buf->index - begin) + 1))
		return (false);
	while (begin != buf->index)
		line->data[line->len++] = buf->buf[begin++];
	line->data[line->len] = '\0';
	return (true);
}

t_buf_read_result	fill_buffer_if_empty(t_buf *buf)
{
	if (buf->index >= (size_t) buf->bytes_read)
	{
		buf->bytes_read = read(buf->fd, buf->buf, BUFFER_SIZE);
		if (buf->bytes_read < 0)
			return (BUF_READ_RESULT_ERROR);
		if (buf->bytes_read == 0)
			return (BUF_READ_RESULT_DONE);
		buf->index = 0;
	}
	return (BUF_READ_RESULT_OK);
}

/**
 * The first call initializes the buffer. 
 * The other calls reset it to make it usable again if:
 * 1. A new FD is provided.
 * 2. The same FD is provided and the last call received the EOF.
 */
bool	buf_init(t_buf *buf, int fd)
{
	if (buf->fd != fd || buf->buf == NULL)
	{
		buf->fd = fd;
		buf->index = 0;
		buf->bytes_read = 0;
	}
	if (buf->buf == NULL)
	{
		buf->buf = malloc(BUFFER_SIZE);
		if (buf->buf == NULL)
			return (false);
	}
	return (true);
}

char	*get_next_line(int fd)
{
	static t_buf		buf;
	t_dynstr			line;
	t_buf_read_result	buf_read_result;

	if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX)
		return (free(buf.buf), buf.buf = NULL, NULL);
	if (!buf_init(&buf, fd))
		return (NULL);
	dynstr_init(&line);
	buf.nl_found = false;
	while (!buf.nl_found)
	{
		buf_read_result = fill_buffer_if_empty(&buf);
		if (buf_read_result == BUF_READ_RESULT_DONE)
			return (free(buf.buf), buf.buf = NULL, line.data);
		if (buf_read_result == BUF_READ_RESULT_ERROR)
			return (free(buf.buf), buf.buf = NULL, free(line.data), NULL);
		if (!append_from_buf_to_line(&buf, &line))
			return (free(line.data), NULL);
	}
	return (line.data);
}

int main(int argc, char **argv)
{
    int fd;
    char *line;

    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }
    while ((line = get_next_line(fd)))
    {
        printf("%s", line);
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] != '\n')
            printf("\n");
        free(line);
    }
    close(fd);
    return 0;
}
