#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (0);
    }
    
    char *str = argv[1];
    int len = 0;
    
    // Find string length - fastest approach
    while (str[len])
        len++;
    
    // Reverse print in a single loop with pre-decrement
    while (len--)
        write(1, &str[len], 1);
    
    write(1, "\n", 1);
    return (0);
}
/*another way to do it ultra fastly*/
/*
#include <unistd.h>

int ft_strlen(char *str)
{
	char *temp;

	temp = str;
	while(*str++);
	return (str - temp - 1);
}

void ft_swap(char *a, char *b)
{
	(*a ^= *b), (*b ^= *a), (*a ^= *b);
}
char *ft_strrev(char *str)
{
	char *start;
	char *end;
	int len;
	
	len = ft_strlen(str);
	start = str;
	end = str + len - 1;
	while(start < end)
		ft_swap(start++, end--);
	return (str);
}

int main(void)
{
	char str[] = "hello world!";
	int len = ft_strlen(str);

	write(1, "original : ", 11);
	write(1, str, len);
	write(1, "\n", 1);

	ft_strrev(str);

	write(1, "reversed : ", 11);
	write(1, str, len);
	write(1, "\n", 1);

	return (0);
}

*/
