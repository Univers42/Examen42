#include <unistd.h>

// Initialize the repeat count lookup table using bitwise operations
void init_repeat_table(unsigned char table[256])
{
    for (int c = 0; c < 256; c++)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            table[c] = (c | 0x20) & 0x1F; // Bitwise position in alphabet (1-26)
        else
            table[c] = 1; // Non-alphabetic chars repeat once
    }
}

// Process and print a string using the repeat lookup table
void repeat_print(char *str, unsigned char table[256])
{
    int i = 0;
    
    while (str[i])
    {
        // Lookup the repeat count for the current character
        unsigned char count = table[(unsigned char)str[i]];
        
        // Print the character 'count' times
        for (unsigned char j = 0; j < count; j++)
            write(1, &str[i], 1);
            
        i++;
    }
    
    // Print newline at the end
    write(1, "\n", 1);
}

int main(int argc, char **argv)
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (0);
    }
    
    // Initialize lookup table
    unsigned char repeat_table[256] = {0};
    init_repeat_table(repeat_table);
    
    // Process and print the input string
    repeat_print(argv[1], repeat_table);
    
    return (0);
}
