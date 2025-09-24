#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

# define GLYPH '*'

static int     ft_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n--)
    {
        if (*s1 != *s2)
            return ((unsigned char)*s1 - (unsigned char)*s2);
        s1++;
        s2++;
    }
    return (0);
}

static void    filter(char *haystack, const char *needle, int c)
{
    char    *writer;
    size_t  needle_len;
    size_t  count;

    needle_len = strlen(needle);
    while (*haystack)
    {
        if (ft_strncmp(haystack, needle, needle_len) == 0)
        {
            writer = haystack;
            count = needle_len;
            while (count-- > 0)
                *writer++ = (char)c;
            haystack += needle_len;
        }
        else
            haystack++;
    }
}

int main(int argc, const char **argv)
{
    const char  *pattern;
    char        *buffer;
    ssize_t     readn;
    int         c;
    size_t      capacity;
    size_t      total_read;
    char        *tmp;
    
    if (argc != 2)
        return (1);
    pattern =  argv[1];
    c = GLYPH;
    capacity = BUFFER_SIZE;
    buffer = calloc(capacity, sizeof(char));
    if (!buffer)
        return (2);
    total_read = 0;
    readn = read(STDIN_FILENO, buffer, capacity);
    while (readn > 0)
    {
        total_read += readn;
        if (total_read >= capacity)
        {
            capacity *= 2;
            tmp = realloc(buffer, capacity);
            if (!tmp)
                return (free(buffer), buffer = NULL, 3);
            buffer = tmp;
        }
        readn = read(STDIN_FILENO, buffer + total_read, capacity - total_read);
    }
    buffer[total_read] = '\0';
    filter(buffer, pattern, c);
    printf("%s", buffer);
    free(buffer);
    return (0);
}
