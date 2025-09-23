#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <unistd.h>

# define SCANNING 1

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# define RESET_BUF(gnl) (free((gnl)), gnl = NULL)
# define BUF_RESET(gnl) RESET_BUF((gnl))
typedef enum e_state
{
    STATE_OK,
    STATE_HAS_NL,
    STATE_RAW_DATAS,
    STATE_NOTHING_TO_READ,
    STATE_ERR
}   t_state;

typedef struct s_file
{
    char    buf[BUFFER_SIZE];
    char    *cur;
    char    *end;
    int     flags;
}   t_file;

typedef struct s_dynstr
{
    char    *buf;
    size_t  cap;
    size_t  size;
}   t_dynstr;

char    *get_next_line(int fd);
void    *ft_realloc(void *ptr, size_t old_size, size_t new_size);
void    *ft_memmove(void *dst, const void *src, size_t n);
void    *ft_memchr(void *ptr, int c, size_t n);

#endif
