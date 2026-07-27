#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "life.h"

int	nb(t_board *b, int x, int y)
{
	int	c = 0, i, j;

	for (i = -1; i < 2; i++)
		for (j = -1; j < 2; j++)
			if ((i || j) && x + j >= 0 && x + j < b->w && y + i >= 0 && y + i < b->h)
				c += b->cells[(y + i) * b->w + x + j];
	return (c);
}

int	main(int ac, char **av)
{
	t_board	b, t;
	char	c;
	int		x = 0, y = 0, p = 0, i, k, n;

	if (ac != 4)
		return (1);
	b.w = atoi(av[1]);
	b.h = atoi(av[2]);
	k = atoi(av[3]);
	b.cells = calloc(b.w * b.h, 1);
	while (read(0, &c, 1) > 0)
	{
		if (c == 'x')
			p = !p;
		y += (c == 's') - (c == 'w');
		x += (c == 'd') - (c == 'a');
		if (p && x >= 0 && x < b.w && y >= 0 && y < b.h)
			b.cells[y * b.w + x] = 1;
	}
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
	for (i = 0; i < b.w * b.h; i++)
	{
		putchar(b.cells[i] ? 'O' : ' ');
		if (i % b.w == b.w - 1)
			putchar('\n');
	}
	return (free(b.cells), 0);
}


