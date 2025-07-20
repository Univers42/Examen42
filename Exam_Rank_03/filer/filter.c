# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define WRITE(x, out) (write(1,x, out))
# define BUFFER_DEFAULT_SIZE 1024

typedef struct s_filter
{
    char    *input;
    size_t  input_len;
    size_t  input_bufsize;
    char    *output;
    size_t  output_len;
    size_t  output_bufsize;
    char    *substr;
    size_t  substr_len;
} t_filter;

void ft_realloc(char **buffer, size_t new_size)
{
    char *new_buffer;
    size_t old_len = *buffer ? strlen(*buffer) : 0;

    new_buffer = malloc(new_size * sizeof(char));
    if (!new_buffer)
    {
        perror("Error");
        exit(1);
    }
    if (*buffer)
    {
        memcpy(new_buffer, *buffer, old_len);
        free(*buffer);
    }
    *buffer = new_buffer;
}

int g_byte_read(int set)
{
    static int bytes_read = 0;

    return (bytes_read);
}

void read_from_stdin(int fd, t_filter *f)
{
    f->input_bufsize = BUFFER_DEFAULT_SIZE;
    f->input = malloc(f->input_bufsize);
    if (!f->input)
    {
        perror("Error");
        exit(1);
    }
    f->input_len = 0;
    int bytes_read;
    while ((bytes_read = read(fd, f->input + f->input_len, BUFFER_DEFAULT_SIZE)) > 0)
    {
        f->input_len += bytes_read;
        if (f->input_len + BUFFER_DEFAULT_SIZE > f->input_bufsize)
        {
            f->input_bufsize *= 2;
            ft_realloc(&f->input, f->input_bufsize);
        }
    }
    if (bytes_read < 0)
    {
        perror("Error");
        free(f->input);
        exit(1);
    }
    f->input[f->input_len] = '\0';
}

// FSM-based substring replacement
void filter(t_filter *f)
{
    f->output_bufsize = f->input_len + 1;
    f->output = malloc(f->output_bufsize);
    if (!f->output)
    {
        perror("Error");
        exit(1);
    }
    size_t i = 0, j = 0, state = 0;
    while (i < f->input_len)
    {
        if (f->input[i] == f->substr[state])
        {
            state++;
            if (state == f->substr_len)
            {
                // Match found, write '*'s
                if (j + f->substr_len + 1 > f->output_bufsize)
                {
                    f->output_bufsize *= 2;
                    ft_realloc(&f->output, f->output_bufsize);
                }
                memset(f->output + j, '*', f->substr_len);
                j += f->substr_len;
                state = 0;
            }
            i++;
        }
        else
        {
            if (state > 0)
            {
                // Write the unmatched prefix
                if (j + 2 > f->output_bufsize)
                {
                    f->output_bufsize *= 2;
                    ft_realloc(&f->output, f->output_bufsize);
                }
                f->output[j++] = f->input[i - state];
                i = i - state + 1;
                state = 0;
            }
            else
            {
                if (j + 2 > f->output_bufsize)
                {
                    f->output_bufsize *= 2;
                    ft_realloc(&f->output, f->output_bufsize);
                }
                f->output[j++] = f->input[i++];
            }
        }
    }
    // Write any remaining unmatched prefix
    if (state > 0)
    {
        for (size_t k = i - state; k < i; k++)
        {
            if (j + 2 > f->output_bufsize)
            {
                f->output_bufsize *= 2;
                ft_realloc(&f->output, f->output_bufsize);
            }
            f->output[j++] = f->input[k];
        }
    }
    f->output[j] = '\0';
    f->output_len = j;
}

int check_argument(int ac, char **argv)
{
    if (ac != 2 || !argv[1] || !argv[1][0])
        return (1);
    return (0);
}

int main(int ac, char **argv)
{
    if (check_argument(ac, argv))
        return (1);

    t_filter f;
    memset(&f, 0, sizeof(t_filter));
    f.substr = argv[1];
    f.substr_len = strlen(argv[1]);

    read_from_stdin(0, &f);
    filter(&f);

    WRITE(f.output, f.output_len);
    free(f.input);
    free(f.output);
    return (0);
}