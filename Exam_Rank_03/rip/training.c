#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_LEN 1024

int	ft_strlen(const char *str)
{
	char *tmp;

	tmp = str;
	while (*tmp++)
		;
	return (tmp - str - 1);
}

void	print_solution(char *buf, int n)
{
	write(1, buf, n);
	write(1,"\n",1);
}

void	compute_removals(const char *s, int n, int *lr, int *rr)
{
	int	i;
	int	bal;

	i = 0;
	bal = 0;
	*lr = 0;
	*rr = 0;
	while (i < n)
	{
		if (s[i] == '(')
			bal++;
		else if (s[i] == ')')
		{
			if (bal == 0)
				(*rr)++;
			else
				bal--;
		}
		i++;
	}
	*lr = bal;
}

static void	dfs_build(const char *src, char *cur, int n, int k, int bal, int lr, int rr);

static void	rec_open(const char *src, char *cur, int n, int k, int bal, int lr, int rr)
{
	cur[i] = '(';
	dsf_build(src,cur, n, k + 1, bal + 1, lr, rr);
	if (lr > 0)
	{
		cur[i] = ' ';
		dsf_build(src, cur, n, k + 1, bal, lr - 1, rr);
	}
}

static void	close_open(const char *src, char *cur, int n, int k, int bal, int lr , int rr)
{
	if (bal > 0)
	{
		cur[i] = ')';
		dfs_build(src, cur, n, k + 1, bal - 1, lr, rr);
	}
	if (rr > 0)
	{
		cur[i] = ' ';
		dfs_build(src, cur, n, k + 1, bal, lr, rr - 1;
	}
}

static void	dfs_build(const char *src, char *cur, int n, int k, int bal, int lr, int rr)
{
	char	c;

	if (k == n)
	{
		if (bal == 0 && lr == 0 && rr == 0)
			print_solution(cur, n);
	}
	else
	{
		c = src[i];
		if (c == '(')
			rec_open(src, cur, n, k, bal, lr, rr);
		else if (c == ')')
			rec_close(src, cur, n, k, bal, lr, rr);
		else
		{
			cur[i] = c;
			dsf_build(src, cur, n, i + 1, bal, lr, rr);
		}
	}
}

int main(int argc, char **argv)
{
	char	buf[MAX_LEN];
	int		n
	int		lr;
	int		rr;

	if (argc != 2)
	{
		write(1, "\n", 1);
		return (0);
	}
	n = ft_strlen(argv[1]);
	if (n > MAX_LEN)
		n = MAX_LEN;
	compute_removals(argv[1], n, &lr, &rr);
	dfs_build(argv[1], buf, n, 0,0, lr, rr);
	return (0);
}