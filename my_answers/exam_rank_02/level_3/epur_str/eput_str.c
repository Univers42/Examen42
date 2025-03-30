#include <unistd.h>

// Check if character is whitespace
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t')

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return 0;
    }
    char *start = argv[1];
    char *end = argv[1];
    int word_found = 0;
    
    while (*start && IS_SPACE(*start))
        start++;
    end = start;
    while (*end)
    {
        while (*end && !IS_SPACE(*end))
            end++;
        if (end > start)
        {
            if (word_found)
                write(1, " ", 1);
            write(1, start, end - start);
            word_found = 1;
        }
        while (*end && IS_SPACE(*end))
            end++;
        start = end;
    }
    write(1, "\n", 1);
    return 0;
}