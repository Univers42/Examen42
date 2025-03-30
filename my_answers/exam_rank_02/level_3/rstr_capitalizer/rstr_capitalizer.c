#include <unistd.h>

// Checks if a character is a letter using bitwise operations
int is_letter(char c)
{
    // Convert to uppercase and check if it's in [A-Z] range
    return ((c & ~32) - 'A') < 26;
}

// Checks if a character is a delimiter using bitwise operations
int is_delimiter(char c)
{
    // Simple and efficient check without shift-count warnings
    return (c == ' ' || c == '\t' || c == '\0');
}

void rstr_capitalizer(char *str)
{
    int i = 0;
    
    while (str[i])
    {
        // Look ahead to determine if this is the last letter of a word
        if (is_letter(str[i]) && (is_delimiter(str[i + 1])))
        {
            // Last letter - convert to uppercase by clearing bit 5
            char c = str[i] & ~32;
            write(1, &c, 1);
        }
        else if (is_letter(str[i]))
        {
            // Not the last letter - convert to lowercase by setting bit 5
            char c = str[i] | 32;
            write(1, &c, 1);
        }
        else
        {
            // Not a letter, write as-is
            write(1, &str[i], 1);
        }
        i++;
    }
}

int main(int argc, char **argv)
{
    int i;
    
    if (argc == 1)
    {
        write(1, "\n", 1);
        return 0;
    }
    
    i = 1;
    while (i < argc)
    {
        rstr_capitalizer(argv[i]);
        write(1, "\n", 1);
        i++;
    }
    
    return 0;
}