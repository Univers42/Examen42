/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 22:02:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/14 22:25:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

# define BUFFER_SIZE 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 100000

int main(int argc, char **argv)
{
    char *line;
    char *needle;
    size_t needle_len, i, j;

    if (argc != 2 || strlen(argv[1]) == 0)
    {
        fprintf(stderr, "Usage: %s <substring>\n", argv[0]);
        return (EXIT_FAILURE);
    }
    needle = argv[1];
    needle_len = strlen(needle);

    line = malloc(MAX_LINE_LENGTH);
    if (!line)
        return (EXIT_FAILURE);

    ssize_t bytes_read = read(STDIN_FILENO, line, MAX_LINE_LENGTH - 1);
    if (bytes_read <= 0)
    {
        free(line);
        return (EXIT_FAILURE);
    }
    line[bytes_read] = '\0';

    for (i = 0; line[i]; i++)
    {
        if (strncmp(&line[i], needle, needle_len) == 0)
        {
            for (j = 0; j < needle_len; j++)
                line[i + j] = '*';
            i += needle_len - 1;
        }
    }
    printf("%s", line);
    free(line);
    return (EXIT_SUCCESS);
}

void read_stdin()
{
    char *line;
    size_t bytes_read;

    bytes_read = read(STDIN_FILENO, line, MAX_LINE_LENGTH - 1);
    if (bytes_read <= 0)
    {
        free(line);
        return (EXIT_FAILURE);
    }
    line[bytes_read] = '\0';
    while (bytes_read)
    {
        size = 
    }
}

void filter(char *haystack, char *needle)
{
    
}

int main(int argc, char **argv)
{
    
    if (argc != 2)
    {
        printf("%s <pattern>", argv[0]);
        return (1);
    }
    read_stdin();
    filter();
    
}