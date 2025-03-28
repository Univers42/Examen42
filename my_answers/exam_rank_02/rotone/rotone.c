#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	char	*str;
	char	c;
	
	if (argc != 2)
		return (write(1, "\n", 1));
	str = argv[1];
	while(*str)
	{
		c = *str;
		// Check if alphabetic (bit 0x40 set as initial filter)
		if (c & 0x40)
		{
			if (c >= 'A' && c <= 'Z')
				c = 'A' + ((c - 'A' + 1) & 0x1F); // increment with a wrap around from Z to A
			else if (c >= 'a' && c <= 'z')
				c = 'a' + ((c - 'a' + 1) & 0x1F); // increment with a wrap around from z to a
		}
		write(1, &c, 1);
		str++;
	}
	write(1, "\n", 1);
	return (0);
}
