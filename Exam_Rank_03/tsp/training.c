/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   training.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 21:44:28 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 21:44:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MAX_CITIES 12

typedef struct s_city
{
	float   x;
	float   y;
}	t_city;

typedef struct s_tsp
{
	t_city  cities[MAX_CITIES];
	int     path[MAX_CITIES];
	int     best_path[MAX_CITIES];
	bool    used[MAX_CITIES];
	int     n;
	float   best_length;
}           t_tsp;

// Swap two integers (city indices)
void ft_swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

// Return true if all cities are visited
bool is_solution(int k, int n)
{
	return (k == n);
}

// Print the best solution found
void print_solution(t_tsp *tsp)
{
	printf("%.2f\n", tsp->best_length);
}

// Build all unused cities
void build_candidate(t_tsp *tsp, int *candidates, int *nc, int n)
{
	*nc = 0;
	for (int i = 0; i < n; ++i)
	{
		if (!tsp->used[i])
			candidates[(*nc)++] = i;
	}
}

void make_move(t_tsp *tsp, int city)
{
	tsp->used[city] = true;
}

void unmake_move(t_tsp *tsp, int city)
{
	tsp->used[city] = false;
}

float path_length(t_tsp *tsp, int *p, int n)
{
	float len = 0.0f;
	for (int i = 0; i < n; ++i)
	{
		int a = p[i];
		int b = p[(i + 1) % n];
		float dx = tsp->cities[a].x - tsp->cities[b].x;
		float dy = tsp->cities[a].y - tsp->cities[b].y;
		len += sqrtf(dx * dx + dy * dy);
	}
	return len;
}

void tsp_backtrack(t_tsp *tsp, int k, int n)
{
	if (is_solution(k, n))
	{
		float len = path_length(tsp, tsp->path, n);
		if (tsp->best_length < 0 || len < tsp->best_length)
		{
			tsp->best_length = len;
			memcpy(tsp->best_path, tsp->path, sizeof(int) * n);
		}
	}
	else
	{
		int candidates[MAX_CITIES];
		int nc;
		build_candidate(tsp, candidates, &nc, n);
		for (int i = 0; i < nc; ++i)
		{
			int city = candidates[i];
			tsp->path[k] = city;
			make_move(tsp, city);
			tsp_backtrack(tsp, k + 1, n);
			unmake_move(tsp, city);
		}
	}
}

int main(int argc, char **argv)
{
	FILE *stream;
	t_tsp tsp;
	tsp.n = 0;
	tsp.best_length = -1.0f;
	memset(tsp.used, 0, sizeof(tsp.used));

	if (argc == 2)
	{
		stream = fopen(argv[1], "r");
		if (!stream)
		{
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	}
	else
		stream = stdin;
	while (tsp.n < MAX_CITIES && fscanf(stream, "%f, %f\n", &tsp.cities[tsp.n].x, &tsp.cities[tsp.n].y) == 2)
		tsp.n++;
	if (stream != stdin)
		fclose(stream);
	if (tsp.n == 0)
		return 0;
	tsp.path[0] = 0;
	tsp.used[0] = true;
	tsp_backtrack(&tsp, 1, tsp.n);
	print_solution(&tsp);
	return 0;
}
