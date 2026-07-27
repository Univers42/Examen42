#ifndef BSQ_H
# define BSQ_H

// malloc, calloc, realloc, free, fopen, fclose,
// getline, fscanf, fputs, fprintf, stderr, stdout, stdin, err

# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# endif

# include <stdio.h>
# include <stdlib.h>

# define BSQ_SEPARATE_MAPS 1

typedef struct s_map {
    char    *grid;
    size_t  rows;
    size_t  cols;
    size_t  stride;
    char    empty;
    char    obstacle;
    char    full;
}t_map;

typedef struct s_ctx {
    t_map   map;
    unsigned int    *dp;
    unsigned int    best;
    size_t           br;
    size_t          bc;
}t_ctx;

int     solve_stream(FILE *f);
void    map_error(void);

#endif