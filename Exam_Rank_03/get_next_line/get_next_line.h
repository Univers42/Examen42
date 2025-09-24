#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

# define RESET_BUF(gnl) (free((gnl)), (gnl) = NULL)

# define SCANNING 1

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif


typedef struct s_file
{
    char    buf[BUFFER_SIZE];
    char    *cur;
    char    *end;
}   t_file;

typedef struct s_dynstr
{
    char    *buf;
    size_t  cap;
    size_t  size;
}   t_dynstr;

void    *ft_memmove(void *dst, const void *src, size_t n);
void    *ft_memchr(void *ptr, int c, size_t n);
void    *ft_realloc(void *ptr, size_t old_size, size_t new_size);
char    *get_next_line(int fd);
#endif
