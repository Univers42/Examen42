#include <unistd.h>

void first_word(char *str)
{
    while (*str & 0xFF && !((*str ^ ' ') & 0xFF))
        str++;
    while (*str & 0xFF && ((*str ^ ' ') & 0xFF))
        write(1, str++, 1);
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
