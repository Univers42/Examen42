/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:58:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/22 00:48:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <fcntl.h>
# include <stdbool.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# define RESET_BUF(line) (free((line)), (line) = NULL)

typedef struct s_file
{
    char    buffer[BUFFER_SIZE];
    char    *cur;
    char    *end;  
}   t_file;

typedef struct s_dynstr
{
    char    *buf;
    size_t  cap;
    size_t  size;    
}   t_dynstr;

char    *get_next_line(int fd);
void    *ft_realloc(void *src, size_t old, size_t new);
void    *ft_memchr(void *src, int c, size_t nbytes);
void    *ft_memmove(void *dst, const void *src, size_t nbytes);

#endif