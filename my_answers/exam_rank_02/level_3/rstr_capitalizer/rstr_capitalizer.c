#include <unistd.h>

// Character classification macros
#define IS_LETTER(c) (((c & ~32) >= 'A') && ((c & ~32) <= 'Z'))
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t')

// Case conversion macros
#define TO_UPPER(c) ((c) & ~32)
#define TO_LOWER(c) ((c) | 32)

// Output macros
#define WRITE_CHAR(c) write(1, &(c), 1)
#define WRITE_NEWLINE() write(1, "\n", 1)

void rstr_capitalizer(char *str)
{
    while (*str)
    {
        if (IS_LETTER(*str))
        {
            if (*(str + 1) == '\0' || IS_SPACE(*(str + 1)))
                *str = TO_UPPER(*str);
            else
                *str = TO_LOWER(*str);
        }
        WRITE_CHAR(*str++);
    }
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return(WRITE_NEWLINE(), 0);
    
    int i = 1;
    while (i < argc)
    {
        rstr_capitalizer(argv[i]);
        WRITE_NEWLINE();
        i++;
    }
    return 0;
}
