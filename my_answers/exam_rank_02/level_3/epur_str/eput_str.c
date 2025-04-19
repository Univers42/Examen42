#include <unistd.h>

#define IS_SPACE(c) (((c) ^ ' ') == 0 || ((c) ^ '\t') == 0)
#define WRITE_CHAR(c) (write(1,&(c),1))

int ft_strlen(char *str)
{
    char *temp;
    temp = str;
    while(*str)
        str++;
    return (str - temp);
}

//"    hello   world"
void epur(char *str)
{
    char *sentinel;
    int printed_space = 0;
    sentinel = str + ft_strlen(str) - 1;

    while (*str && IS_SPACE(*str))
        str++;
    while (sentinel > str && IS_SPACE(*sentinel))
        sentinel--;
    while (str <= sentinel)
    {
        int is_space = IS_SPACE(*str);
        int should_print = is_space ^ printed_space;
        if (should_print || !is_space)
            WRITE_CHAR(*str);
        printed_space = is_space;
        str++;
    }
    WRITE_CHAR("\n");
}

int main(int argc, char **argv)
{
    char *str;
    if(argc != 2)
        return(WRITE_CHAR("\n"), 1);
    str = argv[1];
    epur(str);
    return (0);
}
