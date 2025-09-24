#include "get_next_line.h"

int     ensure_cap(char **line, size_t *cap, size_t need)
{
    size_t  new_cap;
    void    *tmp;

    if (*cap >= need)
        return (1);     //need to verify if the exit code is correct about the other function
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

int     refill(t_file *f, int fd)
{
    ssize_t readn;

    readn = read(fd, f->buf, BUFFER_SIZE);
    if (readn <= 0)
        return ((int)readn);    // EOF
    f->cur = f->buf;
    f->end = f->buf + readn;
    return (1);             // Potentially optional
}

int     append_from_buffer(t_file *f, t_dynstr *line)
{
    char    *nl;
    size_t  avail;
    size_t  chunk;

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

int     scan_nl(t_file *f, t_dynstr *line, int fd)
{
    int st;

    while (SCANNING)
    {
        if (f->cur >= f->end)
        {
            st = refill(f, fd);
            if (st <= 0)
                break;
        }
        st = append_from_buffer(f, line);
        if (st == -1)
            return (RESET_BUF(line->buf), 0);      // we have to return an error not sure about the exit code
        if (st == 1)
            break;
    }
    return (1);
}

char *get_next_line(int fd)
{
    static t_file   f = {{0}, 0, 0};
    t_dynstr        line = {NULL, 0, 0};

    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    if (!scan_nl(&f, &line, fd))
        return (NULL);
    if (line.size == 0)
        return (RESET_BUF(line.buf), NULL);
    return (line.buf);
}

__attribute__((weak))
int main(int argc, const char **argv)
{
    int     fd;
    char    *line;

    if (argc != 2)
        return (1);
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
        return (1);
    line = get_next_line(fd);
    while (line)
    {
        printf("%s", line);
        free(line);
        line = get_next_line(fd);
    }
    close(fd);
    return (0);
}
