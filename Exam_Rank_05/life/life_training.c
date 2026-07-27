#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct s_board {
    int     w;
    int     h;
    char    *cells;
}   t_board;

int main(int ac, char **av)
{
       t_board  b;  // board being drawn simulated
       t_board  t;  // the nexte generation board built each iteration, then swaped into b,
       char     c;  // the single byte read from stdin in the parsing loop ('w','a','s','d','x', or garbage)
       int      x;  // coord horizontal
       int      y;  // coord vert
       int      p;  // pen toggle
       int      i;  // loop index
       int      k;  // iteration countdown
       int      n;  //neigbor count

       // we always want 4 parameters only
       if (ac != 4)
            return (1);
        b.w = atoi(av[1]);
        b.h = atoi(av[2]);
        k = atoi(av[3]);
        b.cells = calloc(b.w * b.h, 1);
        /**
        this is a pen drawing loop that reads stdin byte-by-byte until EOF, moving a virtual pen,
        */
        while (read(0, &c, 1) > 0)
        {
            if (c == 'x')   // toggle pen state (p): drawing off
                p = !p; 
            y += (c == 's') - (c == 'w');   // move down/up (only one of the two conditions is true at a time, or neither)
            x += (c == 'd') - (c == 'a');   // move right/left
            if (p && x >= 0 && x < b.w && y >= 0 && y < b.h)    //if pen is down and the pen is currently inside the board -> mark that cell alive
                b.cells[y * b.w + x] = 1;   // any other (garbage \n) just falls through with no effect -- no x/w/s/a/d match, so nothing moves and nothing gets drawn.
        }
        /**
        now we run the simultation for `k` iterations, one generation per loop:
        - `t.cells = calloc(...)` - allocate a fresh buffer for the next generation
        - for every cell `i` (converted to `x = i & b.w. y = i / b.w`), nb(&b, x, y)` counts its live neighbors in the current board b.
        - Conway's rule: cell is alived next gen if it has exactly 3 neighboars or has 2 neighboars and is already alive (b.cells[i]).
        - `free(b.cells) then b.cells = t.cells` - discard the old generation, swap in the new one as the current board.

        `k--` counts down each pass, so the loop runs exactly `k` times total.
        */
        while (k-- > 0)
        {
            t.cells = calloc(b.w * b.h, 1);
            for (i = 0; i < b.w * b.h; i++)
            {
                n = nb(&b, i % b.w, i / b.w);
                t.cells[i] = (n == 3 || (n == 2 && b.cells[i]));
            }
            free(b.cells);
            b.cells = t.cells;
        }
        /**
        this prints the final board, row by row:
        - for every cell `i`, print '0'  if alive (`b.cells[i]` truthy) else a space
        - `i % b.w == b.w - 1` is true exactly at the last column of each row -> print '\n' there
        to start a new line
        (Note: your)
        */
        for (i = 0; i < b.w * b.h; i++)
        {
            putchar(b.cells[i] ? '0' : ' ');
            if (i % b.w == b.w - 1)
                putchar('\n');
        }
        return (free(b.cells), 0);
}