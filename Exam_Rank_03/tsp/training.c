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

static	void	print_solution(t_tsp *scan)
{
	printf("%.2f", scan->best_length);
}

static	bool	update_best_length(t_tsp *scan, int len, int n)
{
	if (scan->best_length < 0 || scan->best_length > len)
	{
		scan->best_length = len;
		memcpy(scan->best_path, scan->path, sizeof(int) * n);	//not sure about it
		return (true);
	}
	return (false);
}

static float	pythagore_len(t_tsp *scan, int *p, int n)
{
	int		i;
	int		coord[2];
	float	dist[2];
	float	len = 0.0f;		//forget to initialize

	i = -1;
	while (++i < n)
	{
		coord[0] = p[0];
		coord[1] = p[1];
		dist[0] += scan->city[coord[0]].x - scan->city[coord[1]].x;
		dist[1] += scan->city[coord[1]].y - scan->city[coord[1]].y;	//forget to increment
		len = sqrtf(dist[0] * dist[0] + dist[1] * dist[1]);
	}
	return (len);
}

void	build_candidate(t_tsp *scan, int *c, int n, int *nc)
{
	int	i;

	*nc = 0;
	i = -1;
	while (++i < n)
		if (!scan->used[i])
			c[(*nc)++] = i;
}

void	tsp(t_tsp *scan, int k, int n)
{
	int	candidate[MAX_CITIES];
	int	nc;
	int	len;
	int	city;
	int	i;

	if (k == n)
	{
		len = pythagore_len(scan, scan->path, n);
		if (!update_best_length(scan, len, n))
			return ;
	}
	else
	{
		i = -1;
		build_candidate(scan, candidate, n, &nc);
		while (++i < nc)
		{
			city = candidate[i];
			scan->path[k] = city;
			scan->used[i] = true;
			tsp(scan, k + 1, n);
			scan->used[i] = false;
		}
	}
}

int	main(int argc, char **argv)
{
	FILE	*stream;
	int		n;
	t_tsp	scan;

	if (argc == 2)
	{
		stream = fopen(argv[1], "r");
		if (!stream)
		{
			perror("Error");
			return (1);
		}
	}
	else
		stream = stdin;
	memset(&scan, 0, sizeof(t_tsp));
	scan.path[0] = 0;
	scan.best_length = -1.0f;
	scan.used[0] = true;
	n = 0;//forget to initialize
	while (n < MAX_CITIES && fscanf(stream, "%f,%f", &scan.city[n].x, &scan.city[n].y) == 2)
		n++;
	tsp(&scan, 1, n);
	print_solution(&scan);
	return(0);
}