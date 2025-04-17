#include <unistd.h>
# define WRITE_NEW_LINE(c) (write(1,"\n",1))
# define WRITE_CHAR(c) (write(1,&(c),1))
# define IS_UPPER(c) ((c & 0x20) == 0)
# define IS_LOWER(c) ((c & 0x20) != 0)
# define DELTA_COUNT(c, base) ((c) - base + 1)

void repeat_alpha(char *str)
{
	int count;
	int i;
	while(*str)
	{
		count = 1;
		i = 0;
		if(IS_UPPER(*str))
			count = DELTA_COUNT(*str, 'A');
		else if (IS_LOWER(*str))
			count = DELTA_COUNT(*str, 'a');
		while(i < count)
		{
			WRITE_CHAR(*str);
			i++;
		}
		str++;
	}
	WRITE_NEW_LINE();
}

int main(int argc, char **argv)
{
	char *str;
	if (argc != 2)
		return (WRITE_NEW_LINE(), 1);
	str = argv[1];
	if(!str)
		return (WRITE_NEW_LINE(), 1);
	repeat_alpha(str);
}
