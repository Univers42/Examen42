#include <unistd.h>

// Checks if a character is a whitespace
int is_space(char c)
{
    return (c == ' ' || c == '\t');
}

// Writes a word to stdout with efficient single write call
void write_word(char *str, int start, int end)
{
    write(1, str + start, end - start);
}

void rostring(char *str)
{
    int i = 0;
    int first_start = -1, first_end = -1;
    int word_printed = 0;
    
    // Skip leading spaces
    while (str[i] && is_space(str[i]))
        i++;
    
    // Save first word boundaries
    if (str[i])
    {
        first_start = i;
        while (str[i] && !is_space(str[i]))
            i++;
        first_end = i;
    }
    
    // Skip spaces after first word
    while (str[i] && is_space(str[i]))
        i++;
    
    // Process and print the rest of the words
    while (str[i])
    {
        int word_start = i;
        
        // Find end of current word
        while (str[i] && !is_space(str[i]))
            i++;
        
        // If we found a word, print it with space if needed
        if (i > word_start) 
        {
            if (word_printed)
                write(1, " ", 1);
            write_word(str, word_start, i);
            word_printed = 1;
        }
        
        // Skip spaces between words
        while (str[i] && is_space(str[i]))
            i++;
    }
    
    // Print the first word at the end if it exists
    if (first_start != -1 && first_end > first_start)
    {
        if (word_printed)
            write(1, " ", 1);
        write_word(str, first_start, first_end);
    }
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        rostring(argv[1]);
        write(1, "\n", 1);
    }
    else
        write(1, "\n", 1);
    
    return 0;
}