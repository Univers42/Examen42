#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>
# define BUFFER_SIZE 1024
bool is_sort(const char *line)
{
    const char *prev_start = NULL;
    const char *prev_end = NULL;
    const char *curr = line;

    while (*curr)
    {
        // Skip leading spaces
        while (*curr && isspace(*curr))
            curr++;
        if (!*curr)
            break;
        // Mark start of current word
        const char *word_start = curr;
        // Find end of current word
        while (*curr && !isspace(*curr))
            curr++;
        const char *word_end = curr;
        // Compare with previous word
        if (prev_start)
        {
            size_t prev_len = prev_end - prev_start;
            size_t curr_len = word_end - word_start;
            int cmp = 0;
            size_t i = 0;
            while (i < prev_len && i < curr_len)
            {
                if (prev_start[i] != word_start[i])
                {
                    cmp = (unsigned char)prev_start[i] - (unsigned char)word_start[i];
                    break;
                }
                i++;
            }
            if (cmp == 0)
                cmp = (int)prev_len - (int)curr_len;
            if (cmp > 0)
                return false;
        }
        prev_start = word_start;
        prev_end = word_end;
    }
    return true;
}

// Detects if a line is only whitespace (empty subset)
static bool is_blank(const char *s)
{
    while (*s)
    {
        if (!isspace((unsigned char)*s))
            return false;
        s++;
    }
    return true;
}

bool is_valid(const char *line, char **input, int argc)
{
    // input[0] = target, input[1..argc-1] = allowed numbers (in given order)
    const char *curr = line;
    long target = strtol(input[0], NULL, 10);
    long sum = 0;
    int last_idx = 0; // last matched argv index; start before first allowed (which is 1)

    while (*curr)
    {
        // skip spaces
        while (*curr && isspace(*curr))
            curr++;
        if (!*curr)
            break;

        // token bounds
        const char *start = curr;
        while (*curr && !isspace(*curr))
            curr++;
        const char *end = curr;
        int len = (int)(end - start);
        if (len == 0)
            continue;

        // find a matching allowed number at an index >= last_idx + 1
        int found = 0;
        for (int i = last_idx + 1; i < argc; i++)
        {
            if ((int)strlen(input[i]) == len && strncmp(start, input[i], len) == 0)
            {
                sum += strtol(input[i], NULL, 10);
                last_idx = i;
                found = 1;
                break;
            }
        }
        if (!found)
            return false;
    }

    // valid if sum equals target; empty lines (sum==0 unless target==0) will fail naturally
    return (sum == target);
}

int main(int argc, char **argv)
{
    FILE *stream;
    size_t size;
    ssize_t readn;
    char *line = NULL;
    int error = 0;
    long target;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <target> <numbers...>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    stream = fopen(argv[1], "r");
    if (stream == NULL)
        stream = stdin;
    size = BUFFER_SIZE;
    int saw_empty = 0;
    target = strtol(argv[1], NULL, 10);
    readn = getline(&line, &size, stream);
    while (readn != -1)
    {
        if (is_blank(line))
        {
            if (target == 0)
                saw_empty = 1;            // empty subset is valid and must appear
            else
            {
                error = 1;                // empty subset invalid when target != 0
                printf("Invalid: %s", line);
            }
        }
        else if (!is_valid(line, argv + 1, argc - 1))
        {
            error = 1;
            printf("Invalid: %s", line);
        }
        readn = getline(&line, &size, stream);
    }
    if (target == 0 && !saw_empty)
    {
        error = 1;
        printf("Missing empty subset\n");
    }
    if (error)
        printf("Error\n");
    else
        printf("OK!\n");
    free(line);
    fclose(stream);
    return 0;
}