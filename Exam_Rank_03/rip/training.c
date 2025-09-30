#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int	ft_strlen(const char *s)
{
	const char *tmp = s;

	while (*tmp++)
		;
	return (tmp - s - 1);
}

void	compute_removals(const char *src, int n, int *lr, int *rr)
{
	int	bal;
	int	i;

	bal = 0;
	i = -1;
	*lr = 0;
	*rr = 0;
	while (++i < n)
	{
		if (src[i] == '(')
			bal++;
		else if(src[i] == ')')
		{
			if (bal == 0)
				(*rr)++;
			else
				bal--;
		}
	}
	*lr = bal;
}
void	build_dfs(const char *src, char *cur, int n, int k, int bal, int lr, int rr);

void	rec_open(const char *src, char *cur, int n, int k, int bal, int lr, int rr)
{
	cur[k] = '(';
	build_dfs(src, cur, n, k + 1, bal + 1, lr, rr);
	if (lr > 0)
	{
		cur[k] = ' ';
		build_dfs(src, cur, n, k + 1, bal, lr - 1, rr);
	}
}

void	rec_close(const char *src, char *cur, int n, int k, int bal, int lr, int rr)
{
	if (bal > 0)
	{
		cur[k] = ')';
		build_dfs(src, cur, n, k + 1, bal - 1, lr, rr);
	}
	if (rr > 0)
	{
		cur[k] = ' ';
		build_dfs(src, cur, n, k + 1, bal, lr, rr - 1);
	}
}

void	build_dfs(const char *src, char *cur, int n, int k, int bal, int lr, int rr)
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
			rec_open(src, cur, n, k, bal, lr, rr);
		else if (c == ')')
			rec_close(src, cur, n, k, bal, lr, rr);
		else
		{
			cur[k] = c;
			build_dfs(src, cur, n, k + 1, bal, lr, rr);
		}
	}
}

int	main(int argc, char **argv)
{
	char	*src;
	char	*cur;
	int		n;
	int		bal;
	int		lr;
	int		rr;
	int		k;

	if (argc != 2)
		return (1);
	src = argv[1];
	cur = src;
	n = ft_strlen(src);
	bal = 0;
	cur = malloc(sizeof(char) * n);
	if (!cur)
		return (1);
	compute_removals(src, n, &lr, &rr);
	k =0;
	build_dfs(src, cur, n, k, bal, lr, rr);
	free(cur);
	return (1);
}