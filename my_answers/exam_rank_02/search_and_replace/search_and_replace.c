#include <unistd.h>

int main(int argc, char **argv)
{
    unsigned char find;
    unsigned char replace;
    unsigned char lookup[256]; // Fixed: Added array size declaration
    char *str;

    if (argc != 4 || argv[2][1] != '\0' || argv[3][1] != '\0')
        return (write(1, "\n", 1), 0);
    
    find = argv[2][0];
    replace = argv[3][0];
    
    // Initialize lookup table - each character maps to itself
    for (int i = 0; i < 256; i++)
        lookup[i] = i; // Fixed: Don't set every character to replace
    
    // Set up the replacement mapping
    lookup[find] = replace;
    
    str = argv[1];
    while (*str)
    {
        unsigned char c = lookup[(unsigned char)*str];
        write(1, &c, 1);
        str++; // Fixed: Moved the increment outside the lookup to avoid side effects
    }
    
    write(1, "\n", 1);
    return (0);
}
