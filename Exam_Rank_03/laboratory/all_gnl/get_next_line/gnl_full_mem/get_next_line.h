/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:52:39 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/19 21:01:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 50
# endif

# define NL '\n'

/* Bit-mask flags to track reader state */
typedef enum e_gnl_flags
{
	GNL_F_NONE = 0,
	GNL_F_EOF = 1 << 0,
	GNL_F_ERR = 1 << 1
}t_gnl_flags;


/**
 * THOSE MACROS serves the purpose to check | turn on || off a flag
 * 
 */
/* Flag helpers for readability */
/**
 * Checks if the `flags` specified by `B` are set in the flags 
 * field  of struct `f`
 * `(F) -> flags & `B`  performs a bitwise `AND` between the flags and
 * the mask `b`
 * if the result is not zero, it measn at least on of the bits in B is set
 * in `flags`
 * return `true` is the flag is set, false otherwise
 */
# define GNL_HAS(F, B) (((F)->flags & (B)) != 0)

/**
 * 
 */
# define GNL_SET(F, B) ((F)->flags |= (B))

/**
 * `~(B)` creates a mask where all bits in  `B` in the flags field of struct `F`
 * (F)->flags &= ~(B) uses bitwise AND to clear the bits in `B`, leaving other bits
 * unchanged
 */
# define GNL_CLR(F, B) ((F)->flags &= ~(B))

/* Pointer math helpers (use only when base is not NULL) */
/**
 * PURPOSE: calculates the total number of bytes currelyt stored in the buffer
 * f->base
 * f->end
 * base from end gives the number of bytes used
 */
# define GNL_USED(F) ((size_t)((F)->end - (F)->base))
/**
 * calculates the number of bytes that has not been read yet or processed
 f->start point to the current read position in the buffer
 f->end points just past the last valid byte
 substracting `start` from  `end` givese the number of unread bytes
 */
# define GNL_UNREAD(F) ((size_t)((F)->end - (F)->start))
/**
 * Calculates the number of free bytes left at the end of the buffer 
 * (space available for new data)
 */
# define GNL_TAIL_FREE(F) ((F)->cap - GNL_USED(F))

# define GNL_INIT_CAP ((size_t)BUFFER_SIZE * 2)

typedef struct s_file
{
	int		flags;
	char	*start;
	char	*end;
	char	*base;
	size_t	cap;
}t_file;

/* Public API */
char	*get_next_line(int fd);
void	gnl_cleanup(void);

/* Internal helpers used across units */
t_file	*gnl_state(void);
void	gnl_reset(t_file *f);
int		gnl_grow(t_file *f, size_t need);
void	compact_if_needed(t_file *f);
char	*scan_buffer(t_file *f);
char	*drain_tail_or_null(t_file *f);

/* Utils */
void	*ft_memmove(void *dst, const void *src, size_t nbytes);

#endif