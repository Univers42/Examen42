/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 15:39:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 21:21:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

# define  COND_PEER_PARENT(var) ((var) == '(' || (var) == ')')

typedef struct s_state
{
	int		cur_removal;
	int		cur_balance;
	int		min_rem;
	char	*slow_ptr;
	char	*fast_ptr;
	char	*seq;
	int		len;
}			t_state;

static int	ft_strlen(const char *str)
{
	const char	*tmp;

	tmp = str;
	while (*tmp)
		tmp++;
	return ((int)(tmp - str));
}

static char	*preprocess_input(const char *input, t_state *st)
{
	int	alloc_len;
	char	*buf;

	st->slow_ptr = NULL;
	st->fast_ptr = (char *)input;
	alloc_len = ft_strlen(input);
	buf = (char *)malloc((size_t)alloc_len + 1);
	if (!buf)
		return (NULL);
	st->slow_ptr = buf;
	while (*st->fast_ptr)
	{
		if (COND_PEER_PARENT(*st->fast_ptr))
		{
			*st->slow_ptr = *st->fast_ptr;
			st->slow_ptr++;
		}
		st->fast_ptr++;
	}
	*st->slow_ptr = '\0';
	st->seq = buf;
	st->len = ft_strlen(buf);
	return (buf);
}

static void	init_state(t_state *st)
{
	st->cur_removal = 0;
	st->cur_balance = 0;
	st->min_rem = 0;
	st->slow_ptr = NULL;
	st->fast_ptr = NULL;
	st->seq = NULL;
	st->len = 0;
}

static void	compute_min_rem(t_state *st)
{
	st->fast_ptr = st->seq;
	st->cur_balance = 0;
	st->cur_removal = 0;
	while (*(st->fast_ptr))
	{
		if (*(st->fast_ptr) == '(')
			st->cur_balance++;
		else if (*(st->fast_ptr) == ')')
		{
			if (st->cur_balance > 0)
				st->cur_balance--;
			else
				st->cur_removal++;
		}
		st->fast_ptr++;
	}
	st->min_rem = st->cur_removal + st->cur_balance;
	st->cur_removal = 0;
	st->cur_balance = 0;
}

static void	build_candidate(char *seq, int k, int *candidate, int *nc)
{
	if (COND_PEER_PARENT(seq[k]))
	{
		candidate[0] = 1;
		candidate[1] = 0;
		*nc = 2;
	}
	else
	{
		candidate[0] = 1;
		*nc = 1;
	}
}

static void	make_move(char *seq, int *a, int k, int value, t_state *st)
{
	if (COND_PEER_PARENT(seq[k]) && value == 0)
		st->cur_removal++;
	else if (value == 1)
	{
		if (seq[k] == '(')
			st->cur_balance++;
		else if (seq[k] == ')')
			st->cur_balance--;
	}
	a[k] = value;
}

static void	unmake_move(char *seq, int *a, int k, int value, t_state *st)
{
	if (COND_PEER_PARENT(seq[k]) && value == 0)
		st->cur_removal--;
	else if (value == 1)
	{
		if (seq[k] == '(')
			st->cur_balance--;
		else if (seq[k] == ')')
			st->cur_balance++;
	}
	a[k] = 0;
}

static void	print_solution(char *seq, int *a, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (COND_PEER_PARENT(seq[i]) && a[i] == 0)
			write(1, "_", 1);
		else
			write(1, &seq[i], 1);
		write(1, " ", 1);
		i++;
	}
	write(1, "\n", 1);
}

static void	rip(char *seq, int *a, int k, t_state *st)
{
	int	candidate[2];
	int	nc;
	int	i;

	if (st->cur_removal > st->min_rem || st->cur_balance < 0)
		return ;
	if (k == st->len)
	{
		if (st->cur_removal == st->min_rem && st->cur_balance == 0)
			print_solution(seq, a, st->len);
		return ;
	}
	build_candidate(seq, k, candidate, &nc);
	i = 0;
	while (i < nc)
	{
		make_move(seq, a, k, candidate[i], st);
		rip(seq, a, k + 1, st);
		unmake_move(seq, a, k, candidate[i], st);
		i++;
	}
}

int	main(int argc, char **argv)
{
	char		*seq;
	int			*a;
	t_state		st;

	if (argc != 2)
		return (1);
	init_state(&st);
	seq = preprocess_input(argv[1], &st);
	if (!seq)
		return (1);
	compute_min_rem(&st);
	a = (int *)calloc((size_t)st.len, sizeof(int));
	if (!a)
		return (free(seq), 1);
	rip(seq, a, 0, &st);
	free(a);
	free(seq);
	return (0);
}