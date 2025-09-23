#include "get_next_line.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

static int     ensure_cap(char **line, size_t *cap, size_t need)
{
    void    *tmp;
    size_t  new_cap;

    if (*cap >= need)   //>= not >
        return (1);     //success expected, GOOD
    if (*cap)
        new_cap = *cap;
    else
        new_cap = 64;
    while (new_cap < need)
        new_cap *= 2;
    tmp = ft_realloc(*line, *cap, new_cap);
    if (!tmp)
        return (0);     //FAIL EXPECTATION
    *line = (char *)tmp;
    *cap = new_cap;
    return (1);         // good handling
}

static int     append_from_buffer(t_file *f, t_dynstr *line)
{
    char    *nl;
    size_t  chunk;
    size_t  avail;

    //Forget to check the if cur is greater than end
    if (f->cur >=  f->end)
        return (0);
    avail = (size_t)(f->end - f->cur);
    nl = (char *)ft_memchr(f->cur, '\n', avail);
    if (nl)
        chunk = (size_t)(nl - f->cur + 1);
    else
        chunk = avail;
    if (!ensure_cap(&line->buf, &line->cap, line->size + chunk + 1))
        return (-1);
    ft_memmove(line->buf + line->size, f->cur, chunk); //this part to review
    line->size += chunk;                            //this part too
    line->buf[line->size]='\0';                     /// this part too
    f->cur +=  chunk;                               //  this part too
    return (nl != NULL);                            // this part too
}

static int     refill(t_file *f, int fd)
{
    ssize_t     readn;

    readn = read(fd, f->buf, BUFFER_SIZE);
    if (readn <= 0)
        return ((int)readn);
    f->cur = f->buf;
    f->end = f->buf + readn;
    //forget to return that it worked correctly
    return (1);
}

static int scan_nl(t_file *f, t_dynstr *line, int fd)
{
    int st;

    while (SCANNING)
    {
        if (f->cur >= f->end)
        {
            st = refill(f, fd);
            if (st <= 0)
                break ;
        }
        st = append_from_buffer(f, line);
        if (st == -1)
            return (BUF_RESET(line->buf), 0);// WASN'T RETURNING THE GOOD VALUE IT'S 0 NOT NULL
        if (st == 1)                         // st == 1 != st == 0, we want to verify it it success , should put the  condition  == 0 if it's false and catch this error 
            break;
    }
    return (1);
}

char    *get_next_line(int fd)
{
    static t_file      gnl = {{0}, 0, 0, 0};
    t_dynstr    line = {NULL, 0, 0};

    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    if (!scan_nl(&gnl, &line, fd))
        return (NULL);
    if (line.size == 0)
        return (RESET_BUF(line.buf), NULL);     //Completely forgot to reset the line.buf
    return (line.buf);
}

int main(int argc, char **argv)
{
    char    *line;
    int     fd;

    if (argc != 2)
        return (1);
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
        return  (2);
    line = get_next_line(fd);
    while (line)
    {
        printf("%s", line);
        free(line);
        line = get_next_line(fd);
    }
    close(fd);
    return(0);
}
