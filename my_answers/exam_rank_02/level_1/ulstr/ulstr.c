#include <unistd.h>

int main(int argc, char **argv)
{
	char *str;
	char c;

	if (argc != 2)
		return (write(1, "\n", 1), 0);
	str = argv[1];
	while (*str)
	{
		c = *str;
		if (c & 0x40)
		{
			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
			{
				//toggle case with XOR - The most efficient way to flip case
				c ^= 0x20;
			}
		}
		write(1, &c, 1);
		str++;	
	}
	write(1, "\n", 1);
	return (0);
}
