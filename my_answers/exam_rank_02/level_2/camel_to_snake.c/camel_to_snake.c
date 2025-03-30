#include "stdlib.h"

/**
 * Efficient camel_to_snake implementation with: 
 * - Bitwise operations
 * - O(1) time complexity per character
 * - dynamic memory allocation
 */
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char *s;
    char *result;
    int len = 0, i = 0, j = 0;

    if (argc != 2)
        return (write(1, "\n", 1), 0);
    
    s = argv[1];
    
    // Count uppercase letters using bitwise check
    while (s[len])
    {
        // Ensure character is alphabetic and check 6th bit
        if (((s[len] | 32) >= 'a' && (s[len] | 32) <= 'z') && 
            ((s[len] & 32) == 0))
            i++; // Count uppercase letters
        len++;
    }
    
    result = (char *)malloc(len + i + 1);
    if (!result)
        return (write(1, "\n", 1), 0);
        
    i = 0;
    j = 0;
    
    while (s[i])
    {
        // Detect uppercase using bitwise operation
        if (((s[i] | 32) >= 'a' && (s[i] | 32) <= 'z') && 
            ((s[i] & 32) == 0))
        {
            result[j++] = '_';
            result[j++] = s[i] | 32; // Set the 6th bit to convert to lowercase
        }
        else
            result[j++] = s[i];
        i++;
    }
    
    result[j] = '\0';
    write(1, result, j);
    write(1, "\n", 1);
    
    free(result);
    return (0);
}