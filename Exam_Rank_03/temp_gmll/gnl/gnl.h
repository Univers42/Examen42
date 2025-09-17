/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 11:16:43 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/17 14:12:31 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GNL_H
# define GNL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

char *ft_strchr(char *str, int c);
void    *ft_memmove(void *dst, const void *src, size_t n);
void    *ft_realloc(void *ptr, size_t oldsize, size_t newsize);
bool    strclen(char *line, int c, int *size);
char *get_next_line(int fd);

#endif
