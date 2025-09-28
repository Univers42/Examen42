#include <unistd.h>
# define WRITE_NEW_LINE() (write(1,"\n",1))
# define WRITE_NEW_CHAR(c) (write(1,&(c),1))
# define SAME_CHAR(c,s) ((c ^ s) == 0)
void search_and_replace(char *str, char sub, char by)
{
	while(*str)
	{
		if (SAME_CHAR(*str, sub))
			WRITE_NEW_CHAR(by);
		else
			WRITE_NEW_CHAR(*str);
		str++;
	}
	WRITE_NEW_LINE();
}

int main(int argc, char **argv)
{
	char *str;
	char sub;
	char by;
	if (argc != 4)
		return (WRITE_NEW_LINE(), 1);
	str = argv[1];
	sub = (char)argv[2][0];
	by = (char)argv[3][0];
	search_and_replace(str, sub, by);
}
