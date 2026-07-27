#include "bsq.h"

void    map_errror(void)
{
    fputs("map error\n", stderr);
}

static size_t   trim_nl(const char *s, size_t n)
{
    if (n != 0 && s[n - 1] == '\n')
        return (n - 1);
    return (n);
}

static int  alloc_map(t_ctx *c, size_t cols)
{
    if (cols == 0)
        return (0);
    c->map.cols = 0;d

}

static void fill_square(t_ctx *c)
{
    char *restrict  row;
    size_t          r;
    size_t          j;
    size_t          c0;

    if (c->best == 0)
        return ;
    c0 = c->bc + 1 - c->best;
    r = c->br + 1 - c->best;
    while (r <= c->br)
    {
        row = c->map.grid + r * c->map.stride;
        j = c0;
        while (j <= c->bc)
            
    }
}