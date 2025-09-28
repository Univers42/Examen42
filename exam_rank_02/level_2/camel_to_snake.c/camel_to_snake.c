#include <unistd.h>

# define NEWLINE() (write(1,"\n",1))
# define WRITE_CHAR(c) (write(1,&(c),1))

void camel_to_snake(char *str)
{
    while (*str)
    {
        if (*str >= 'A' && *str <= 'Z')
        {
			char underline = '_';
            WRITE_CHAR(underline);
            char lower = *str + 32;
            WRITE_CHAR(lower);
        }
        else
            WRITE_CHAR(*str);
        str++;
    }
    NEWLINE();
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (NEWLINE(), 1);
    camel_to_snake(argv[1]);
    return 0;
}
