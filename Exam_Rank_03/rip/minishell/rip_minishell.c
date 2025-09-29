#include <unistd.h>

#define MAX_LEN 4096

typedef struct s_ctx
{
	int		lrem[3];
	int		rrem[3];
	int		qrem[3];
	int		stack[MAX_LEN];
	int		top;
	char	quote;
}	t_ctx;

static int		ft_strlen(const char *s);
static int		idx_in(const char *set, char c);
static void		print_solution(char *buf, int n);
static void		ctx_zero(t_ctx *c);
static void		compute_removals_multi(const char *s, int n, t_ctx *c);
static void		rec_quote(const char *src, char *cur, int n, int i, t_ctx *c);
static void		rec_open(const char *src, char *cur, int n, int i, t_ctx *c, int oi);
static void		rec_close(const char *src, char *cur, int n, int i, t_ctx *c, int ci);
static void		dfs_multi(const char *src, char *cur, int n, int i, t_ctx *c);

static const char	g_open[] = "([{";
static const char	g_close[] = ")]}";
static const char	g_quote[] = "'\"`";

static int	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

static int	idx_in(const char *set, char c)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

static void	print_solution(char *buf, int n)
{
	write(1, buf, n);
	write(1, "\n", 1);
}

static void	ctx_zero(t_ctx *c)
{
	int	i;

	i = 0;
	while (i < 3)
	{
		c->lrem[i] = 0;
		c->rrem[i] = 0;
		c->qrem[i] = 0;
		i++;
	}
	c->top = 0;
	c->quote = 0;
}

static void	compute_removals_multi(const char *s, int n, t_ctx *c)
{
	int	i;
	int	oi;
	int	ci;
	int	qi;

	ctx_zero(c);
	i = 0;
	while (i < n)
	{
		if (c->quote)
		{
			if (s[i] == c->quote)
				c->quote = 0;
			i++;
			continue ;
		}
		qi = idx_in(g_quote, s[i]);
		if (qi >= 0)
		{
			c->quote = s[i];
			i++;
			continue ;
		}
		oi = idx_in(g_open, s[i]);
		ci = idx_in(g_close, s[i]);
		if (oi >= 0)
			c->stack[c->top++] = oi;
		else if (ci >= 0)
		{
			if (c->top > 0 && c->stack[c->top - 1] == ci)
				c->top--;
			else
				c->rrem[ci]++;
		}
		i++;
	}
	while (c->top > 0)
		c->lrem[c->stack[--c->top]]++;
	if (c->quote)
		c->qrem[idx_in(g_quote, c->quote)]++;
}

static void	rec_quote(const char *src, char *cur, int n, int i, t_ctx *c)
{
	int	qi;

	if (c->quote)
	{
		if (src[i] == c->quote)
		{
			cur[i] = src[i];
			c->quote = 0;
			dfs_multi(src, cur, n, i + 1, c);
			c->quote = cur[i];
			qi = idx_in(g_quote, src[i]);
			if (c->qrem[qi] > 0)
			{
				cur[i] = ' ';
				c->qrem[qi]--;
				dfs_multi(src, cur, n, i + 1, c);
				c->qrem[qi]++;
			}
		}
		else
		{
			cur[i] = src[i];
			dfs_multi(src, cur, n, i + 1, c);
		}
		return ;
	}
	qi = idx_in(g_quote, src[i]);
	if (qi >= 0)
	{
		cur[i] = src[i];
		c->quote = src[i];
		dfs_multi(src, cur, n, i + 1, c);
		c->quote = 0;
		if (c->qrem[qi] > 0)
		{
			cur[i] = ' ';
			c->qrem[qi]--;
			dfs_multi(src, cur, n, i + 1, c);
			c->qrem[qi]++;
		}
	}
}

static void	rec_open(const char *src, char *cur, int n, int i, t_ctx *c, int oi)
{
	cur[i] = src[i];
	c->stack[c->top++] = oi;
	dfs_multi(src, cur, n, i + 1, c);
	c->top--;
	if (c->lrem[oi] > 0)
	{
		cur[i] = ' ';
		c->lrem[oi]--;
		dfs_multi(src, cur, n, i + 1, c);
		c->lrem[oi]++;
	}
}

static void	rec_close(const char *src, char *cur, int n, int i, t_ctx *c, int ci)
{
	if (c->top > 0 && c->stack[c->top - 1] == ci)
	{
		cur[i] = src[i];
		c->top--;
		dfs_multi(src, cur, n, i + 1, c);
		c->top++;
	}
	if (c->rrem[ci] > 0)
	{
		cur[i] = ' ';
		c->rrem[ci]--;
		dfs_multi(src, cur, n, i + 1, c);
		c->rrem[ci]++;
	}
}

static void	dfs_multi(const char *src, char *cur, int n, int i, t_ctx *c)
{
	int	oi;
	int	ci;
	int	qi;

	if (i == n)
	{
		if (c->top == 0 && c->quote == 0
			&& c->lrem[0] == 0 && c->lrem[1] == 0 && c->lrem[2] == 0
			&& c->rrem[0] == 0 && c->rrem[1] == 0 && c->rrem[2] == 0
			&& c->qrem[0] == 0 && c->qrem[1] == 0 && c->qrem[2] == 0)
			print_solution(cur, n);
		return ;
	}
	if (c->quote || idx_in(g_quote, src[i]) >= 0)
	{
		rec_quote(src, cur, n, i, c);
		return ;
	}
	qi = idx_in(g_quote, src[i]);
	oi = idx_in(g_open, src[i]);
	ci = idx_in(g_close, src[i]);
	if (oi >= 0)
		rec_open(src, cur, n, i, c, oi);
	else if (ci >= 0)
		rec_close(src, cur, n, i, c, ci);
	else
	{
		cur[i] = src[i];
		dfs_multi(src, cur, n, i + 1, c);
	}
}

int	main(int argc, char **argv)
{
	t_ctx	c;
	int		n;
	char	buf[MAX_LEN];

	if (argc != 2)
	{
		write(1, "\n", 1);
		return (0);
	}
	n = ft_strlen(argv[1]);
	if (n > MAX_LEN)
		n = MAX_LEN;
	compute_removals_multi(argv[1], n, &c);
	dfs_multi(argv[1], buf, n, 0, &c);
	return (0);
}