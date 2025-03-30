#include <unistd.h>

// Capitalize a lowercase letter using bitwise operations
// Lowercase to uppercase: Clear the 5th bit (32)
#define TO_UPPER(c) ((c) & ~(1 << 5))

// Check if a character is lowercase using bitwise operations
#define IS_LOWER(c) (((c) >= 'a' && (c) <= 'z'))

int main(int argc, char **argv)
{
    char *str;
    int i = 0;
    int capitalize_next = 0;

    if (argc != 2)
    {
        write(1, "\n", 1);
        return 0;
    }
    str = argv[1];
    while (str[i])
    {
        if (str[i] == '_')
            capitalize_next = 1;
        else
        {
            if (capitalize_next && IS_LOWER(str[i]))
            {
                char upper = TO_UPPER(str[i]);
                write(1, &upper, 1);
            }
            else
                write(1, &str[i], 1);
            capitalize_next = 0;
        }
        i++;
    }
    write(1, "\n", 1);
    return 0;
}