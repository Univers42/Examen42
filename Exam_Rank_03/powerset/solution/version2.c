#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

# define MAX_CANDIDATES 2

typedef struct s_seen
{
	char	**arr;
	size_t	n;
	size_t	cap;
}			t_seen;

static t_seen	*seen_store(void)
{
	static t_seen	s =
	{
		NULL,
		0,
		0
	};
	return (&s);
}

static int ensure_seen_cap(size_t need)
{
	char	**tmp;
	size_t	cap;
	t_seen	*st = seen_store();

	if (st->cap >= need)
		return (1);
	cap = st->cap ? st->cap : 16;
	while (cap < need)
		cap <<= 1;
	tmp = (char **)realloc(st->arr, cap * sizeof(*st->arr));
	if (!tmp)
		return 0;
	st->arr = tmp;
	st->cap = cap;
	return 1;
}

static char *xstrdup(const char *s)
{
	size_t	len;
	char	*p;

	len = strlen(s) + 1;
	p = (char *)malloc(len);
	if (!p)
		return NULL;
	memcpy(p, s, len);
	return (p);
}

static int already_seen(const char *s)
{
	t_seen	*st = seen_store();
	size_t	i = 0;

	while (i < st->n)
	{
		if (strcmp(st->arr[i], s) == 0)
			return (1);
		i++;
	}
	return (0);
}

static int remember_line(const char *s)
{
	t_seen	*st = seen_store();
	char	*dup;

	if (already_seen(s))
		return (0);
	if (!ensure_seen_cap(st->n + 1))
		return (-1);
	dup = xstrdup(s);
	if (!dup)
		return (-1);
	st->arr[st->n++] = dup;
	return (1);
}

static size_t int_len(int x)
{
	size_t	len;

	len = 0;
	if (x <= 0)
	{
		len++;
		if (x == 0)
			return (1);
		x = -x;
	}
	while (x)
	{
		len++;
		x /= 10;
	}
	return (len);
}

static char *write_int(char *p, int x)
{
	char	buf[12];
	int		i;
	int		neg;

	i = 0;
	neg = (x < 0);
	if (x == 0)
	{
		*p++ = '0';
		return (p);
	}
	if (neg)
		x = -x;
	while (x)
	{
		buf[i++] = (char)('0' + (x % 10));
		x /= 10;
	}
	if (neg)
		*p++ = '-';
	while (i--)
		*p++ = buf[i];
	return (p);
}

static char *build_line(int *nums, int *a, int n)
{
	size_t total = 0;
	int count = 0;
	int i;

	i = -1;
	while (++i < n)
		if (a[i])
		{
			if (count)
				total++;
			total += int_len(nums[i]);
			count++;
		}
	// empty subset => empty string
	char *s = (char *)malloc(total + 1);
	if (!s)
		return NULL;
	char *p = s;
	count = 0;
	i = -1;
	while (++i < n)
		if (a[i])
		{
			if (count)
				*p++ = ' ';
			p = write_int(p, nums[i]);
			count++;
		}
	*p = '\0';
	return (s);
}

static void	free_seen(void)
{
	t_seen *st = seen_store();
	size_t i = 0;

	while (i < st->n)
		free(st->arr[i++]);
	free(st->arr);
	st->arr = NULL;
	st->n = 0;
	st->cap = 0;
}

bool	is_solution(int target, int *nums, int *a, int n)
{
	int	i;
	int	res;

	res = 0;
	i = -1;
	while (++i < n)
		if (a[i])
			res += nums[i];
	return (res == target);
}

void	print_solution(int *nums, int *a, int n)
{
	char	*s;
	int		st;

	s = build_line(nums, a, n);
	if (!s)
		return ;
	st = remember_line(s);
	if (st == 1)
		printf("%s\n", s);
	// if st == 0, it was a duplicate; if -1, allocation failed -> silently skip
	free(s);
}

void    powerset(int target, int *nums, int *a, int k, int n)
{
	int	candidate[MAX_CANDIDATES];
	int	nc;
	int	i;

	if (k == n)
	{
		if (is_solution(target, nums, a, n))
			print_solution(nums, a, n);
	}
	else
	{
		i = -1;
		candidate[0] = 1;
		candidate[1] = 0;
		nc = 2;
		while (++i < nc)
		{
			a[k] = candidate[i];
			powerset(target, nums, a, k + 1, n);
		}
	}
}

int main(int argc, char **argv)
{
	int target;
	int *nums;
	int *a;
	int k;
	int n;
	int i;

	if (argc < 3)
		return (1);
	target = atoi(argv[1]);
	n = argc - 2;
	a = calloc(n, sizeof(int));
	if (!a)
		return (2);
	nums = calloc(n, sizeof(int));
	if (!nums)
		return (free(a), 3);
	k = 0;
	i = -1;
	while (++i < n)
		nums[i] = atoi(argv[i + 2]);
	powerset(target, nums, a, k, n);
	free_seen();
	return (free(a), free(nums), 0);
}

