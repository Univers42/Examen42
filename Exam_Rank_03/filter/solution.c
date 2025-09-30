#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef BUFFER_SIZE 
# define BUFFER_SIZE 102400000
#endif

# define GLYPH '*'

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	while (n--)
	{
		if (*s1 != *s2)
			return ((unsigned char)*s1 - (unsigned char)*s2);
		s1++;
		s2++;
	}
	return (0);
}

void	filter(char *haystack, char *needle, int c)
{
	char	*writer;
	size_t	needle_len;
	size_t	count;

	needle_len = strlen(needle);
	while (*haystack)
	{
		if (ft_strncmp(haystack, needle, needle_len) == 0)
		{
			writer = haystack;
			count = needle_len;
			while (count-- > 0)
				*writer++ = (char)c;
		}
		else
			haystack++;
	}
}

int main(int argc, char **argv)
{
	char    *pattern;
	int     c;

	char	*temp;
	size_t	capacity;
	size_t	total_read;
	ssize_t	readn;
	char	*buffer;

	if (argc != 2)
		return (1);
	capacity = BUFFER_SIZE;
	buffer = calloc(capacity, sizeof(char));
	if (!buffer)
		return (perror("Error"), 2);
	readn = read(STDIN_FILENO, buffer, BUFFER_SIZE);
	total_read = 0;
	while(readn)
	{
		total_read += readn;
		if (total_read > capacity)
		{
			capacity *= 2;
			temp = realloc(buffer, capacity);
			if (!temp)
				return (perror("Error"), 2);
			buffer = temp;
		}
		readn = read(STDIN_FILENO, buffer + total_read, capacity - total_read + 1);
	}
	c = GLYPH;
	pattern =  argv[1];
	buffer[total_read] = '\0';
	filter(buffer, pattern, c);
	printf("%s", buffer);
	free(buffer);
	return (0);
}