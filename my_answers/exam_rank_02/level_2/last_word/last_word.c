#include <unistd.h>

// Properly formatted macros with parentheses
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t')

int main(int argc, char **argv)
{
    char *str;
    int i, start, end;

    if (argc != 2)
        return (write(1, "\n", 1), 0);
    str = argv[1];
    i = 0;
    start = -1;
    end = -1;
    while (str[i])
    {
        if (!IS_SPACE(str[i]) && (i == 0 || IS_SPACE(str[i - 1])))
            start = i;
        if (!IS_SPACE(str[i]) && (str[i + 1] == '\0' || IS_SPACE(str[i + 1])))
            end = i;    
        i++;
    }
    if (start != -1 && end != -1)
        write(1, &str[start], end - start + 1);
    write(1, "\n", 1);
    return (0);
}
