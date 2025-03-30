#include <unistd.h>

enum e_formula
{
    LOWER = 219,
    UPPER = 155,
};
/**
 * Check if alphabetica character using bitwise OR to normalize case
 * WE use bitwise AND to check if lowercase (6th bit is set)
 * Apply appropriate mirror formula based on case
 * Binary representation of 'A' is 01000001
 * Binary representation of 32 is: 00100000
 * 'A' | 32 = 01100001 = 'a'
 * FOR CASE DETECTION
 * Example : For uppercase 'M' (01001101)
 * 1. 01001101
 * 2. 00100000
 * ' M' | 32 = 01101101 & 001000000 = 00000000 = 0 (fase, uppercase)
 * 
 * The mirror magic happens through this formula:access
 * lowercase: 219 - c <=> 'a' + 'z' -c = 219
 * uppercase: 'A' + 'Z' -c <=> 65 + 90 - c <=> 155 - c
 * first we check if the character is lowercase or uppercase
 * Then we apply the formula: 
 * M & 32 = 0 (uppercase) --> current result
 * m & 32 = 1 (lowercase)
 * apply formula : 155 - 77 = 78 = 'N'
 */
int main(int argc, char **argv)
{
    char    *str;
    char    c;

    if(argc != 2)
        return (write(1, "\n", 1), 0);
    
    str = argv[1];
    while(*str)
    {
        c = *str;
        if ((c | 32) >= 'a' && (c | 32) <= 'z')
            c = (c & 32) ? LOWER - c : UPPER - c;
        write(1, &c, 1);
        str++;
    }
    write(1, "\n", 1);
    return (0);
}