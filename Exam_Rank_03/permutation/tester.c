#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
# define BUFFER_SIZE 100000

int main(void)
{
    char    prev[BUFFER_SIZE];
    char    cur[BUFFER_SIZE];
    int     not_ordered = 0;
    size_t  len;

    if (!fgets(prev, sizeof(prev), stdin))
        return 0;
    len = strlen(prev);
    if (len && prev[len - 1] == '\n')
        prev[len - 1] = '\0';
    while (fgets(cur, sizeof(cur), stdin))
    {
        len = strlen(cur);
        if (len && cur[len - 1] == '\n')
            cur[len - 1] = '\0';
        if (strcmp(prev, cur) > 0)
        {
            printf("KO\n");
            not_ordered = 1;
            break;
        }
        strcpy(prev, cur);
    }
    if (!not_ordered)
        printf("OK!\n");
    return 0;
}