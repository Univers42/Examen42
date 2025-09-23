#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
# define BUFFER_SIZE 100000

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        if (*s1 != *s2)
            return ((unsigned char)*s1 - (unsigned char)*s2);
        s1++;
        s2++;
    }
    return ((unsigned char)*s1 - (unsigned char)*s2);
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    char prev[BUFFER_SIZE];
    char cur[BUFFER_SIZE];
    int not_ordered = 0;

    if (!fgets(prev, sizeof(prev), stdin))
        return 0; // no input
    size_t len = strlen(prev);
    if (len && prev[len - 1] == '\n') prev[len - 1] = '\0';

    while (fgets(cur, sizeof(cur), stdin))
    {
        len = strlen(cur);
        if (len && cur[len - 1] == '\n') cur[len - 1] = '\0';
        if (ft_strcmp(prev, cur) > 0)
        {
            printf("not ordered alphabetically\n");
            not_ordered = 1;
            break;
        }
        // move current to prev for next comparison
        strcpy(prev, cur);
    }

    if (!not_ordered)
        printf("ordered alphabetically\n");
    return 0;
}