#include <unistd.h>
#include <string.h>

char *ft_strrev(char *str)
{
    char *start = str;
    char *end = str;

    if (!*str)
        return (str);
    while (*(end + 1))
        end++;
    while (start < end)
    {
        *start ^= *end;
        *end ^= *start;
        *start ^= *end;
        start++;
        end--;
    }
    return (str);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (write(1, "\n", 1), 0);
    char *str = argv[1];
    char *result = ft_strrev(str);
    write(1, result, strlen(result));
    write(1, "\n", 1);
    return (0);
}