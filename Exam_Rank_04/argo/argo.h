#ifndef ARGO_H
# define ARGO_H

# include <stdio.h>

typedef struct json {
    enum { NUMBER, STRING, MAP } type;
    union {
        int number;
        char *string;
        struct map *map;
    } value;
} json;

struct map {
    char *key;
    json value;
    struct map *next;
};

int argo(json *dst, FILE *stream);

#endif