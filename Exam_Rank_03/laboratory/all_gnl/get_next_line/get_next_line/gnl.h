/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:44:01 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/15 10:50:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GNL_H
#define GNL_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

char *ft_strjoin(const char *s1, const char *s2);
void *ft_memmove(void *dst, const void *src, size_t len);
char *ft_strndup(const char *str, int n);
bool strclen(const char *str, int c, int *size);

#endif