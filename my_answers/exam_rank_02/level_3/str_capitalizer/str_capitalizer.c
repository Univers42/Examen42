#include <unistd.h>

void str_capitalizer(char *str)
{
    int i = 0;
    int new_word = 1; // Track the start of a word

    while (str[i])
    {
        // Check if character is a letter using bitwise operation
        char c = str[i];
        int is_letter = ((c & ~32) - 'A') < 26;

        if (is_letter)
        {
            if (new_word)
                c &= ~32; // Convert to uppercase (clear bit 5)
            else
                c |= 32;  // Convert to lowercase (set bit 5)
        }

        write(1, &c, 1);
        
        // Update new_word flag for next iteration
        // Current char is delimiter -> next char starts a new word
        new_word = (str[i] == ' ' || str[i] == '\t');
        i++;
    }
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        write(1, "\n", 1);
        return 0;
    }
    
    int i = 1;
    while (i < argc)
    {
        str_capitalizer(argv[i]);
        write(1, "\n", 1);
        i++;
    }
    
    return 0;
}