#include <unistd.h>
# define NEWLINE() (write(1,"\n",1))
# define TOGGLE(c) (c ^= 0x20)

int main(int argc, char **argv)
{
	char *str;
	char ch;
	if(argc == 2)
	{
		str = argv[1];
		while (*str)
		{
			ch = TOGGLE(*str);
			write(1, &ch, 1);
			str++;
		}
	}
	return (NEWLINE(),1);
}
