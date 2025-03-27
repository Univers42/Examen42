#include <unistd.h>

void ft_putstr(char *start, char *end)
{
    while (start < end)
    {
        write(1, start, 1);
        start++;
    }
}

int main(int argc, char **argv)
{
    char *str;
    char *start;
    char *end;

    if (argc > 1)
    {
        str = argv[1];
        while (*str == ' ' || *str == '\t') 
            str++;
        start = str;
        while (*str && *str != ' ' && *str != '\t')
            str++;
        end = str;

        while (*str == ' ' || *str == '\t')
            str++;

        while (*str)
        {
            if (*str == ' ' || *str == '\t')
            {
                while (*str == ' ' || *str == '\t')
                    str++;
                if (*str)
                    write(1, " ", 1);
            } 
            else 
                write(1, str++, 1);
        }

        if (start < end)
        {
            write(1, " ", 1);
            ft_putstr(start, end);
        }
    }
    write(1, "\n", 1);
    return 0;
}
