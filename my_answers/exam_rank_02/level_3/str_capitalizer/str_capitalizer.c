#include <unistd.h>

// Character classification macros
#define IS_LETTER(c) (((c & ~32) >= 'A') && ((c & ~32) <= 'Z'))
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t')

// Case conversion macros
#define TO_UPPER(c) ((c) & ~32)
#define TO_LOWER(c) ((c) | 32)
// Output macro
#define WRITE_CHAR(c) write(1, &(c), 1)
#define WRITE_NEWLINE() write(1, "\n", 1)

void str_capitalizer(char *str)
{
    int capitalize = 1;

    while (*str)
    {
        if (IS_LETTER(*str))
        {
            *str = capitalize ? TO_UPPER(*str) : TO_LOWER(*str);
            capitalize = 0;
        }
        else if (IS_SPACE(*str))
            capitalize = 1;
        WRITE_CHAR(*str);
        str++;
    }
}

int main(int argc, char **argv)
{
    int i = 1;
    if (argc == 1)
        return(WRITE_NEWLINE(), 0);
    while (i < argc)
    {
        str_capitalizer(argv[i]);
        WRITE_NEWLINE();
        i++;
    }
    return (0);
}
