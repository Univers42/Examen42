#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 100000

int match_at(char *haystack, char *needle)
{
    while (*needle)
    {
        if (*haystack != *needle)
            return (0);
        haystack++;
        needle++;
    }
    return (1);
}

void convert_to_pattern(char *start, int len, char pattern)
{
    int i = 0;
    while (i < len)
    {
        start[i] = pattern;
        i++;
    }
}

char *filter(char *haystack, char *needle)
{
    size_t i;
    size_t needle_len;

    needle_len = strlen(needle);
    i = 0;

    while (haystack[i])
    {
        if (match_at(&haystack[i], needle))
        {
            convert_to_pattern(&haystack[i], needle_len, '*');
            i += needle_len; // Skip past the replaced substring
        }
        else
        {
            i++;
        }
    }
    return (haystack);
}


int main(int argc, char **argv)
{
    char    *substring;
    char    *result;
    size_t  bytes_read;
    char    *line;

    if (argc != 2 || strlen(argv[1]) == 0)
    {
        perror("Usage: my_filter <substring>\n");
        return (EXIT_FAILURE);
    }
    substring = argv[1];
    line = malloc(MAX_LINE_LENGTH);
    bytes_read = read(STDIN_FILENO, line, MAX_LINE_LENGTH);
    result = filter(line, substring);
    printf("bytes read[%zu] -> %s\n", bytes_read, result);
    return (EXIT_SUCCESS);
}