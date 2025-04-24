#include <unistd.h>

#define IS_SPACE(x) ((x) == ' ' || (x) == '\t' || (x) == '\n')
#define NEWLINE() (write(1, "\n", 1))

int ft_strlen(char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return (len);
}


void rev_wstr(char *str)
{
    int size;
    char *faster;
    char *slower;

    size = ft_strlen(str);
    faster = str + size - 1;
    slower;
    while(faster >= str)
    {
        while (IS_SPACE(*faster) && faster >= str)
            faster--;
        if (faster < str)
            break;
        slower = faster;
        while(!IS_SPACE(*slower) && slower >= str)
            slower--;
        write(1,slower + 1,(faster - slower));
        if (slower > str)
            write(1," ",1);
        faster = slower - 1;
    }
    NEWLINE();
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (NEWLINE(), 1);
    rev_wstr(argv[1]);
    return 0;
}