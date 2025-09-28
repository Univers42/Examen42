#include <unistd.h>

// Character classification macro
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t')

// Output macros
#define WRITE_CHAR(c) do { char _c = (c); write(1, &_c, 1); } while(0)
#define WRITE_STR(s, len) write(1, s, len)
#define WRITE_NEWLINE() write(1, "\n", 1)
#define WRITE_N_SPACES() write(1, "   ", 3)

void expand_str(char *str)
{
    int word_printed = 0;
    while (*str && IS_SPACE(*str))
        str++;
    while (*str)
    {
        if (IS_SPACE(*str))
        {
            while (*str && IS_SPACE(*str))
                str++;
            if (*str && word_printed)
                WRITE_N_SPACES();
        }
        else
        {
            WRITE_CHAR(*str);
            word_printed = 1;
            str++;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        WRITE_NEWLINE();
        return 0;
    }
    expand_str(argv[1]);
    WRITE_NEWLINE();
    return 0;
}
