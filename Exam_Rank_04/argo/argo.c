#include "argo.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Forward declaration
int parse_value(json *dst, FILE *stream);

void error(int c) {
    if (c == EOF)
        printf("Unexpected end of input\n");
    else
        printf("Unexpected token '%c'\n", (char)c);
    exit(1);
}

int parse_string(char **dst, FILE *stream) {
    int c;
    int i = 0;
    int size = 16;
    char *s = malloc(size);

    while ((c = getc(stream)) != EOF && c != '"') {
        // Ensure space for up to 2 chars (escape + char) + null terminator
        if (i + 2 >= size) {
            size *= 2;
            s = realloc(s, size);
        }
        
        if (c == '\\') {
            s[i++] = c; // Store the backslash!
            c = getc(stream);
            if (c != '"' && c != '\\') error(c);
        }
        s[i++] = c;
    }
    
    if (c == EOF) error(c);
    s[i] = '\0';
    *dst = s;
    return 1;
}

int parse_map(struct map **dst, FILE *stream) {
    int c = getc(stream);
    
    // Check for immediate empty map '{}'
    if (c == '}') {
        *dst = NULL;
        return 1;
    }
    ungetc(c, stream); // Put back the first char if not '}'

    struct map *head = NULL, **curr = &head;

    while (1) {
        c = getc(stream);
        // A key must strictly start with '"' (No trailing commas allowed)
        if (c != '"') error(c);
        
        *curr = calloc(1, sizeof(struct map));
        parse_string(&(*curr)->key, stream);
        
        // REPAIR 1: Store getc() in a variable so we don't double-read on error
        int colon = getc(stream);
        if (colon != ':') error(colon);

        if (parse_value(&(*curr)->value, stream) == -1) return -1;
        
        c = getc(stream);
        if (c == '}') break;
        if (c != ',') error(c);
        
        // Move to the next node pointer
        curr = &(*curr)->next;
    }
    *dst = head;
    return 1;
}

int parse_value(json *dst, FILE *stream) {
    int c = getc(stream);
    // REPAIR 2: Call error(EOF) explicitly instead of a silent return
    if (c == EOF) error(EOF);

    if (isdigit(c) || c == '-') {
        ungetc(c, stream);
        dst->type = NUMBER;
        if (fscanf(stream, "%d", &dst->value.number) != 1) return -1;
    } else if (c == '"') {
        dst->type = STRING;
        parse_string(&dst->value.string, stream);
    } else if (c == '{') {
        dst->type = MAP;
        parse_map(&dst->value.map, stream);
    } else {
        error(c);
    }
    return 1;
}

int argo(json *dst, FILE *stream) {
    if (parse_value(dst, stream) == 1)
        return 1;
    return -1;
}

// ==========================================
// TESTER MAIN (Unchanged)
// ==========================================

void print_json(json *node) {
    if (node->type == NUMBER) {
        printf("%d", node->value.number);
    } 
    else if (node->type == STRING) {
        printf("\"%s\"", node->value.string);
    } 
    else if (node->type == MAP) {
        printf("{");
        struct map *curr = node->value.map;
        while (curr) {
            printf("\"%s\":", curr->key);
            print_json(&curr->value);
            if (curr->next) printf(",");
            curr = curr->next;
        }
        printf("}");
    }
}

void free_json(json *node) {
    if (node->type == STRING) {
        free(node->value.string);
    } 
    else if (node->type == MAP) {
        struct map *curr = node->value.map;
        while (curr) {
            struct map *next = curr->next;
            free(curr->key);
            free_json(&curr->value);
            free(curr);
            curr = next;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) return 1;

    FILE *stream = fopen(argv[1], "r");
    if (!stream) return 1;

    json ast;
    if (argo(&ast, stream) == 1) {
        print_json(&ast);
        free_json(&ast);
    }

    fclose(stream);
    return 0;
}