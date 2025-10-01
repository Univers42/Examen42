#include "exam.h"
// only puts and write I can use

int	ft_strlen(const char *src)
{
	const char *tmp = src;

	while (*tmp++)
		;
	return  (tmp - src - 1);
}

void	compute_removals(const char *src, int *lr,  int *rr, int n)
{
	int	i;
	int	bal;

	*lr = 0;
	*rr = 0;
	i = -1;
	bal = 0;
	while (++i < n)
	{
		if (src[i] == '(')
			bal++;
		else if (src[i] ==  ')')
		{
			if (bal == 0)
				(*rr)++;
			else
				bal--;
		}
	}
	*lr = bal;
}

static void	dfs_build(char *src, char *cur, int k, int n, int bal, int lr,  int rr);

void	rec_open(char *src, char *cur, int k, int n, int bal, int lr,  int rr)
{	
	cur[k] = '(';
	dfs_build(src, cur, k + 1, n, bal + 1, lr, rr);
	if (lr > 0)
	{
		cur[k] = ' ';
		dfs_build(src, cur, k + 1, n, bal, lr - 1, rr);
	}
}

void	rec_close(char *src, char *cur, int k, int n, int bal, int lr,  int rr)
{
	if (bal > 0)
	{
		cur[k] = ')';
		dfs_build(src, cur, k + 1, n, bal - 1, lr, rr);
	}
	if (rr > 0)
	{
		cur[k] = ' ';
		dfs_build(src, cur, k + 1, n, bal, lr, rr - 1);
	}
}

static void	dfs_build(char *src, char *cur, int k, int n, int bal, int lr,  int rr)
{
	char	c;

	if (k == n)
	{
		if (bal == 0 && lr == 0 && rr == 0)
		{
			write(STDOUT_FILENO, cur, n);
			write(STDOUT_FILENO, "\n", 1);
		}
	}
	else
	{
		c = src[k];
		if (c == '(')
			rec_open(src, cur, k, n, bal, lr, rr);
		else if (c == ')')
			rec_close(src, cur, k, n, bal, lr, rr);
		else
		{
			cur[k] = c;
			dfs_build(src, cur, k + 1, n, bal, lr, rr);
		}
	}
}

int	main(int argc, char **argv)
{
	char	*src;
	int		k;
	int		n;
	int		bal;
	int		lr;
	int		rr;
	int		i;

	if (argc != 2)
		return (1);
	src = argv[1];
	n = ft_strlen(src);
	src = argv[1];
	char	cur[n];
	i = -1;
	while (++i < n)
		cur[i] = src[i];
	k = 0;
	bal = 0;
	compute_removals(src, &lr, &rr, n);
	dfs_build(src, cur, k, n, bal, lr, rr);
	return (0);
}