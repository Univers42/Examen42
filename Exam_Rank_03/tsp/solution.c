/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 23:32:55 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/29 20:10:20 by dlesieur         ###   ########.fr       */
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

typedef struct s_tsp
{
	t_city	cities[MAX_CITIES];
	int		path[MAX_CITIES];
	int		best_path[MAX_CITIES];
	bool	used[MAX_CITIES];
	int		n;
	float	best_length;
}	t_tsp;

void print_solution(t_tsp *tsp)
{
	printf("%.2f\n", tsp->best_length);
}

void	build_candiate(t_tsp *tsp, int *c, int *nc, int n)
{
	int	i;

	*nc = 0;
	i = -1;
	while (++i < n)
	{
		if (!tsp->used[i])
			c[(*nc)++] = i;
	}
}

bool	terminate(t_tsp *tsp, float len, int n)
{
	if (tsp->best_length < 0 || len < tsp->best_length)
	{
		tsp->best_length = len;
		memcpy(tsp->best_path, tsp->path, sizeof(int) * n);
		return (true);
	}
	return (false);
}

float	path_length(t_tsp *tsp, int *p, int n)
{
	float	len = 0.0f;
	int		i;
	int		coord[2];
	float	dist[2];

	i = -1;
	while (++i < n)
	{
		coord[0] = p[i];
		coord[1] = p[(i + 1) % n];
		dist[0] = tsp->cities[coord[0]].x - tsp->cities[coord[1]].x;
		dist[1] = tsp->cities[coord[0]].y - tsp->cities[coord[1]].y;
		len += sqrtf(dist[0] * dist[0] + dist[1] * dist[1]);
	}
	return (len);
}

// Rename tsp to tsp_func to avoid shadowing struct name
void	tsp_func(t_tsp *tsp, int k, int n)
{
	float	len;
	int		candidates[MAX_CITIES];
	int		nc;
	int		i;
	int		city;

	if (k == n)
	{
		len = path_length(tsp, tsp->path, n);
		terminate(tsp, len, n);
	}
	else
	{
		build_candiate(tsp, candidates, &nc, n);
		i = -1;
		while (++i < nc)
		{
			city = candidates[i];
			tsp->path[k] = city;
			tsp->used[city] = true;
			tsp_func(tsp, k + 1, n);
			tsp->used[city] = false;
		}
	}
}

int	main(int argc, char **argv)
{
	FILE	*stream;
	t_tsp	tsp;
	int		i;

	memset(&tsp, 0, sizeof(t_tsp));
	tsp.best_length = -1.0f;
	if (argc != 2)
		return (1);
	stream = fopen(argv[1], "r");
	if (!stream)
	{
		perror("Error : ");
		return (1);
	}
	i = 0;
	while (i < MAX_CITIES &&
		fscanf(stream, "%f,%f", &tsp.cities[i].x, &tsp.cities[i].y) == 2)
		i++;
	tsp.n = i;
	fclose(stream);
	if (tsp.n == 0)
		return (0);
	tsp.path[0] = 0;
	tsp.used[0] = true;
	tsp_func(&tsp, 1, tsp.n);
	print_solution(&tsp);
	return (0);
}