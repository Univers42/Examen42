#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

# define MAX_LINE_LENGTH 100000

bool is_match(char *haystack, char *needle, int *end)
{
	char *temp_haystack;
	char *temp_needle;

	*end = 0;
	temp_haystack = haystack;
	temp_needle = needle;
	while(*temp_haystack && *temp_needle)
	{
		if (*temp_haystack != *temp_needle)
			return (false);
		temp_haystack++;
		temp_needle++;
		(*end)++;
	}
	return (*temp_needle == '\0');
}

void over_write(char *line, char pattern, int *end)
{
	int i;

	i = -1;
	while (++i < *end)
		line[i] = pattern;
}

void filter(char *haystack, char *needle)
{
	int end;
	char *line;

	line = haystack;
	while(*haystack)
	{
		if (*haystack == *needle)
			if (is_match(haystack, needle, &end))
				over_write(haystack, '*', &end);
		haystack++;
	}
	printf("%s\n", line);
}

int main(int argc, char **argv)
{
	char	*line;
	size_t	read_bytes;
	char	*pattern;

	if (argc != 2)
	{
		printf("%s <pattern>", argv[0]);
		return (1);
	}
	line = malloc(MAX_LINE_LENGTH);
	if (!line)
		return (EXIT_FAILURE);
	read_bytes = read(STDIN_FILENO, line, MAX_LINE_LENGTH - 1);
	if (read_bytes <= 0)
	{
		free(line);
		return (EXIT_FAILURE);
	}
	line[read_bytes] = '\0';
	pattern = argv[1];
	filter(line, pattern);
	free(line);
	return (0);
}