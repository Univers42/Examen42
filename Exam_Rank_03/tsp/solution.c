#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

# define MAX_CITIES 12

typedef struct  s_city
{
	float	x;
	float	y;
}	t_city;

typedef struct s_tsp
{
	t_city	city[MAX_CITIES];
	int		path[MAX_CITIES];
	int		best_path[MAX_CITIES];
	bool	used[MAX_CITIES];
	float	best_length;	
}	t_tsp;

static void     *ft_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    if (d == s)
        return (dst);
    while (n--)
        *d++ = *s++;
    return (dst);
}

static void    build_candidate(t_tsp *scan, int *c, int n, int *nc)
{
    int i;

    i = -1;
    *nc = 0;
    while (++i < n)
    {
        if (!scan->used[i])
            c[(*nc)++] = i;
    }
}

static float	pythagore_len(t_tsp *scan, int *p, int n)
{
	int		coord[2];
	float	dist[2];
	float	len;
	int		i;

	len = 0.0f;
	i = -1;
	while (++i < n)
	{
		coord[0] = p[i];
		coord[1] = p[(i + 1) % n];
		dist[0] = scan->city[coord[0]].x - scan->city[coord[1]].x;
		dist[1] = scan->city[coord[0]].y - scan->city[coord[1]].y;
		len += sqrtf(dist[0] * dist[0] + dist[1] * dist[1]);
	}
	return (len);
}

void	tsp(t_tsp *scan, int k, int n)
{
	int     candidate[MAX_CITIES];
	int     nc;
	float	len;
	int     city;
	int     i;

	if (k == n)
	{
		len = pythagore_len(scan, scan->path, n);
		if (scan->best_length < 0 || scan->best_length > len)
        {
            scan->best_length = len;
            ft_memcpy(scan->best_path, scan->path, n * sizeof(scan->path[0]));
        }
		return ;
	}
	i = -1;
	build_candidate(scan, candidate, n, &nc);
	while (++i < nc)
	{
		city = candidate[i];
		scan->path[k] = city;
		scan->used[city] = true;
		tsp(scan, k + 1, n);
		scan->used[city] = false;
	}
}

int	main(int argc, char **argv)
{
	FILE	*stream;
	int		n;
	t_tsp	scan = {{0}, {0}, {0}, {false}, -1.0f};

	if (argc != 2)
		return (1);
	stream = fopen(argv[1], "r");
	if (!stream)
		return (perror("Error"), 1);
	n = 0;
	while (n < MAX_CITIES && fscanf(stream, "%f,    %f", &scan.city[n].x, &scan.city[n].y) == 2)
		n++;
	scan.used[0] = true;
	tsp(&scan, 1, n);
	printf("%.2f\n", scan.best_length);
	fclose(stream);
	return (0);
}