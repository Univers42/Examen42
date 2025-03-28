#include <unistd.h>

enum e_bin
{
    SPACE_FLAG = 0b00100000,
    ASCII_MASK = 0b01111111
};

void first_word(char *str)
{
    char    *word_start;
    int     word_len;
    
    while (*str && (*str & ASCII_MASK) == SPACE_FLAG)
        str++;
    word_start = str;
    while (*str && !((*str & ASCII_MASK) == SPACE_FLAG))
        str++;
    word_len = str - word_start;    
    if (word_len > 0)
        write(1, word_start, word_len);
    write(1, "\n", 1);
}

int main(int argc, char **argv)
{
    if (argc == 2)
        first_word(argv[1]);
    else
        write(1, "\n", 1);
    return 0;
}
