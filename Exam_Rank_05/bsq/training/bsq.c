#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct s_bsq
{
    char            **grid;     // remmebr it's not int it's char
    int             rows;
    int             cols;
    unsigned char   empty;
    unsigned char   full;
    unsigned char   obst;
}   t_bsq;

//  ! the rows is wron
static void free_rows(char **rows, int n)
{
    while (n--)
        free(rows[n]);
    free(rows);
}


static bool parse_header(const char *buf, ssize_t len, t_bsq *b)
{
    int i = 0;
    while(buf[i] >= '0' && buf[i] <= '9' && b->rows < 200000000)
        b->rows = b->rows * 10 + (buf[i++] - '0');  // !forgot to incremnent
    if (i == 0 || b->rows < 1 || i + 7 != len || buf[i] != ' ' || buf[i + 2] != ' ' || buf[i + 4] != ' ' || buf[i + 6] != '\n')
        return (0);
    b->empty = buf[i + 1];
    b->obst = buf[i + 3];   // !obstacle in second positon
    b->full = buf[i + 5];
    return (b->empty != b->full && b->empty != b->obst && b->full != b->obst);
}

static bool read_map(FILE *f, t_bsq *b)
{
    char *buf = NULL;
    size_t cap = 0;
    size_t i = 0;
    ssize_t read = getline(&buf, &cap, f);  // !forgot htat hte char param was char** sowe need to do &
    if (read < 8 || !parse_header(buf, read, b))
        return (free(buf), 0);
    b->grid = calloc((size_t)b->rows, sizeof(char *));
    if (!b)
        return (free(buf), 0);
    while (i < b->rows)   // here maybe the condition is wrong
    {
        ssize_t len = getline(&buf, &cap, f);
        if (len < 1)
            break ;
        if (buf[len - 1] == '\n')
            buf[--len] = '\0';
        if (len < 1)
            break;
        if (i == 0)
            b->cols = len;
        if (b->cols != len)
            break;
        //! we lacked of this process after then
        b->grid[i++] = buf;      // !forgot
        buf = NULL;             // !forgot
        cap = 0;                // !forgot
    }
    if (i == b->rows && getline(&buf, &cap, f) == -1)
        return (free(buf), 1);
    free(buf);              //! forgot
    free_rows(b->grid, i);  //!forgot
    return (0); 
}


static int	solve(t_bsq *b)
{
	int	*dp = calloc((size_t)b->cols + 1, sizeof(int));
	int	best = 0, br = 0, bc = 0;

	if (!dp)
		return (0);
	for (int i = 0; i < b->rows; i++)
	{
		int	diag = 0;

		for (int j = 0; j < b->cols; j++)
		{
			int	up = dp[j + 1];

			if (b->grid[i][j] == b->empty)
			{
				int	v = up < dp[j] ? up : dp[j];

				if (diag < v)
					v = diag;
				// we store the value we've just calculated
				dp[j + 1] = ++v;
				if (v > best)
				{
					best = v;
					br = i;
					bc = j;
				}
			}
			else if (b->grid[i][j] == b->obst)
				dp[j + 1] = 0;
			else
				return (free(dp), 0);
			diag = up;
		}
	}
	free(dp);
	for (int i = br - best + 1; i <= br; i++)
		for (int j = bc - best + 1; j <= bc; j++)
			b->grid[i][j] = b->full;
	return (1);
}

static void bsq(FILE *f)
{
    t_bsq b;

    memset(&b, 0, sizeof(b));
    if (!read_map(f, &b))
    {
        fputs("map error\n", stderr);
        return ;
    }
    if (solve(&b))
    {
        for (int i = 0; i < b.rows; i++)
        {
            fputs(b.grid[i], stdout);
            fputs("\n", stdout);
        }
    }
    // !forgot this part whihch is in case solve fail we must out an error but also free the map that has already been built
    else
        fputs("map aaerror\n", stderr);
    free_rows(b.grid, b.rows);
}

int main(int argc, char **argv)
{
    if (argc < 2){
        bsq(stdin);
        return (0);
    }
    for (size_t i = 1; i < argc; i++)
    {
        FILE *f = fopen(argv[i], "r");
        if (!f)
        {
            fputs("map errddor\n", stderr);
            continue ;
        }
        bsq(f);
        fclose(f);
    }
    return (0);
}
