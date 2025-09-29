#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct s_scan
{
	const char	*cur;
	long		target;
	long		sum;
	int			last_idx;
	int			saw_token;
}	t_scan;

static int	match_token(const char *start, const char *end,
						char **nums, int nnums, int last_idx,
						long *val_out, int *idx_out)
{
	int		i;
	int		len;

	len = (int)(end - start);
	i = last_idx + 1;
	while (i < nnums)
	{
		if ((int)strlen(nums[i]) == len && strncmp(start, nums[i], len) == 0)
		{
			*val_out = strtol(nums[i], NULL, 10);
			*idx_out = i;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	is_valid_line(const char *line, char **numbers, int nnums,
						   long target, int *saw_empty)
{
	t_scan		sc;
	const char	*start;
	const char	*end;
	long		val;
	int			idx;

	sc.cur = line;
	sc.target = target;
	sc.sum = 0;
	sc.last_idx = -1;
	sc.saw_token = 0;
	while (*sc.cur)
	{
		while (*sc.cur && isspace((unsigned char)*sc.cur))
			sc.cur++;
		if (!*sc.cur)
			break;
		start = sc.cur;
		while (*sc.cur && !isspace((unsigned char)*sc.cur))
			sc.cur++;
		end = sc.cur;
		if (!match_token(start, end, numbers, nnums, sc.last_idx, &val, &idx))
			return (0);
		sc.sum += val;
		sc.last_idx = idx;
		sc.saw_token = 1;
	}
	if (!sc.saw_token)
	{
		if (target == 0)
		{
			*saw_empty = 1;
			return (1);
		}
		return (0);
	}
	return (sc.sum == target);
}

int	main(int argc, char **argv)
{
	char	*line;
	size_t	size;
	ssize_t	readn;
	int		error;
	int		saw_empty;
	long	target;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <target> <numbers...>\n", argv[0]);
		return (1);
	}
	target = strtol(argv[1], NULL, 10);
	line = NULL;
	size = 0;
	error = 0;
	saw_empty = 0;
	while ((readn = getline(&line, &size, stdin)) != -1)
	{
		if (!is_valid_line(line, argv + 2, argc - 2, target, &saw_empty))
		{
			error = 1;
			printf("Invalid: %s", line);
		}
	}
	if (target == 0 && !saw_empty)
	{
		error = 1;
		printf("Missing empty subset\n");
	}
	printf("%s\n", error ? "Error" : "OK!");
	free(line);
	return (0);
}