#include <unistd.h>
#define WRITE_CHAR(c) (write(1,&(c),1))

void ft_putstr(char *str)
{
	int len = 0;
	while (str[len++]);
	write(1, str, len - 1);
}

int main(void)
{
	char *str;
	str = "hello world";
	ft_putstr(str);
	return (0);
}
