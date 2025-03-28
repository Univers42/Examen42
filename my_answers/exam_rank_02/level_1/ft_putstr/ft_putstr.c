#include <stdio.h>
#include <unistd.h>


void ft_putstr(char *str)
{
	char *cpy;
	int count;

	cpy = str;
	count = 0;
	while(*str & 0xFF)
		str++;
	count = str - cpy;
	write(1, cpy, count);
	write(1, "\n", 1);
}

int main(int argc, char **argv)
{
	(void)argc;
	ft_putstr(argv[1]);
	return (0);
}

