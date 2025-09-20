#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 100000
#endif

# define MAX_LINE    70000

char	*ft_strdup(char *src)
{
	char	*dest;
	int		i;

	i = 0;
	while (src[i])
		i++;
	dest = (char *)malloc(sizeof(char) * (i + 1));
	i = -1;
	while (src[++i])
		dest[i] = src[i];
	dest[i] = '\0';
	return (dest);
}

char *get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE];
	char		line[MAX_LINE];
	static int	buffer_pos = 0;
	static int	buffer_read = 0;
	int			i;
	bool		lookup = true;

	i = 0;
	if (fd < 0 || BUFFER_SIZE < 0)
		return (NULL);
	while (lookup)
	{
		if (buffer_pos >= buffer_read)
		{
			//we read the file
			buffer_read = read(fd, buffer, BUFFER_SIZE);
			buffer_pos = 0;
			if (buffer_read <= 0)
				break;
		}
		line[i++] = buffer[buffer_pos++];
		if (line[i - 1] == '\n')
			break;
	}
	if (i == 0)
		return  (NULL);
	line[i] = '\0';
	return (ft_strdup(line));
}

int main()
{
	int fd = open("file.txt", O_RDWR);
	char *line;

	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	return (0);
}