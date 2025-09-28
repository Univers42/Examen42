#include <unistd.h>
#define NEWLINE() (write(1,"\n",1))
#define ISSPACE(c) (((c) ^ '\t') == 0 || ((c) ^ ' ') == 0)

void ft_putchar_sp(char *str, int len)
{
    write(1, str, len);
}

int ft_strlen(char *str)
{
    char *temp;
    temp = str;
    while(*str)
        str++;
    return(str - temp);
}

void last_word(char *str)
{
    int len = ft_strlen(str);
    char *end = str + len - 1;
    char *start;

    while(end >= str && ISSPACE(*end))
        end--;
    if (end < str)
        return;
    start = end;
    while(start > str && !ISSPACE(*(start-1)))
        start--;
    ft_putchar_sp(start, end - start + 1);
}

int main(int argc, char **argv)
{
    char *str;
    if(argc == 2)
    {
        str = argv[1];
        last_word(str);
    }
    NEWLINE();
    return(0);
}
