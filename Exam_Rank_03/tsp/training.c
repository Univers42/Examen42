/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   training.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 21:44:28 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/29 20:42:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

# define MAX_CITIES 12

typedef struct s_city
{
	float	x;
	float	y;
}	t_city;

typedef struct s_state
{
	t_city	city[MAX_CITIES];
	int		best_path[MAX_CITIES];
	int		path[MAX_CITIES];
	bool	used[MAX_CITIES];
	float	best_length;
}	t_tsp;

static void	print_solution(t_tsp *scan)
{
	printf("%.2f\n", scan->best_length);
}

static bool	best_len(t_tsp *scan, int n, float len)
{
	if (scan->best_length < 0 || len < scan->best_length) // the condition is right only if I compare the best length not the len
	{
		scan->best_length = len;
		memcpy(scan->best_path, scan->path, sizeof(int) * n);
		return (true);
	}
	return (false);
}

//pytahgore algorithm to fin the best length
static float found_len(t_tsp *scan, int *p, int n)
{
	float	len = 0.0f;
	int		i;
	int		coord[2] = {0};
	float	dist[2] = {0};

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

void	build_candidate(t_tsp *scan, int *c, int *nc, int n)
{
	int	i;

	*nc = 0;
	i = -1;
	while (++i < n)
		if (!scan->used[i])
			c[(*nc)++] = i;
}

static void	tsp(t_tsp *scan, int k, int n)
{
	int		candidate[MAX_CITIES];
	int		nc;
	int		i;
	float	len;
	int		city;

	if (k == n)
	{
		len = found_len(scan, scan->path, n);
		if (!best_len(scan, n, len))
			return ;
	}
	else
	{
		build_candidate(scan, candidate, &nc, n);
		i = -1;
		while (++i < nc)
		{
			city = candidate[i];		//those two lines I tend to forget them
			scan->path[k] = city;		// this one two
			scan->used[city] = true;
			tsp(scan, k + 1, n);
			scan->used[city] = false;
		}
	}
}


int main(int argc, char **argv)
{
	FILE	*stream;
	t_tsp	scan;
	int		n;

	if (argc > 2)
		return (1);
	if (argc == 2)
	{
		stream = fopen(argv[1], "r");
		if (!stream)
		{
			perror("Error : ");
			return (2);
		}
	}
	else
		stream = stdin;
	memset(&scan, 0, sizeof(t_tsp));
	scan.best_length = -1.0f;
	n = 0;
	while (n < MAX_CITIES && fscanf(stream, "%f,%f", &scan.city[n].x, &scan.city[n].y) == 2)
		n++;
	if (n == 0)
		return (3);
	scan.used[0] = true;		// this line is important
	scan.path[0] = 0;
	if (stream != stdin)
		fclose(stream);
	tsp(&scan, 1, n);
	print_solution(&scan);
	return (0);
}