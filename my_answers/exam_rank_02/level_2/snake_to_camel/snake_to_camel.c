#include <unistd.h>
#define WRITE_CHAR(c) (write(1,&(c),1))
#define IS_UNDERLINE(c) (((c) ^ '_') == 0)

void ft_snake_to_camel(char *str)
{
    while(*str)
    {
        if (IS_UNDERLINE(*str))
        {
            str++;
            *str ^= 0x20;
            WRITE_CHAR(*str);
        }
        else
            WRITE_CHAR(*str);
        str++;
    }
    WRITE_CHAR("\n");
}

int main(int argc, char **argv)
{
    char *str;
    if(argc != 2)
        return (WRITE_CHAR("\n"), 1);
    str = argv[1];
    ft_snake_to_camel(str);
    return(0);
}
