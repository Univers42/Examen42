#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

typedef struct {
    char *data;
    size_t len;
    size_t capacity;
} buffer_t;

static void error_exit(const char *msg) {
    perror(msg);
    exit(1);
}

static void buffer_init(buffer_t *buf) {
    buf->data = malloc(BUF_SIZE);
    if (!buf->data)
        error_exit("Error");
    buf->len = 0;
    buf->capacity = BUF_SIZE;
}

static void buffer_ensure_capacity(buffer_t *buf, size_t needed) {
    if (buf->len + needed > buf->capacity) {
        while (buf->len + needed > buf->capacity)
            buf->capacity *= 2;
        buf->data = realloc(buf->data, buf->capacity);
        if (!buf->data)
            error_exit("Error");
    }
}

static void buffer_append_char(buffer_t *buf, char c) {
    buffer_ensure_capacity(buf, 1);
    buf->data[buf->len++] = c;
}

static void buffer_append_stars(buffer_t *buf, size_t count) {
    buffer_ensure_capacity(buf, count);
    memset(buf->data + buf->len, '*', count);
    buf->len += count;
}

static void read_stdin(buffer_t *input) {
    ssize_t bytes_read;
    
    while ((bytes_read = read(0, input->data + input->len, BUF_SIZE)) > 0) {
        input->len += bytes_read;
        buffer_ensure_capacity(input, BUF_SIZE);
    }
    if (bytes_read < 0)
        error_exit("Error");
}

static void filter_and_write(const buffer_t *input, const char *pattern, size_t pattern_len) {
    buffer_t output;
    buffer_init(&output);
    
    size_t i = 0;
    while (i < input->len) {
        size_t match_len = 0;
        
        // Check for pattern match
        while (match_len < pattern_len && 
               i + match_len < input->len && 
               input->data[i + match_len] == pattern[match_len]) {
            match_len++;
        }
        
        if (match_len == pattern_len) {
            // Full match found
            buffer_append_stars(&output, pattern_len);
            i += pattern_len;
        } else {
            // No match, copy single character
            buffer_append_char(&output, input->data[i]);
            i++;
        }
    }
    
    write(1, output.data, output.len);
    free(output.data);
}

int main(int argc, char **argv) {
    if (argc != 2 || !argv[1] || !argv[1][0])
        return 1;
    
    buffer_t input;
    buffer_init(&input);
    
    read_stdin(&input);
    filter_and_write(&input, argv[1], strlen(argv[1]));
    
    free(input.data);
    return 0;
}