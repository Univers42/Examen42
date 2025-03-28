#include <unistd.h>

int main(int argc, char **argv)
{
    char *str;
    char c;

    if (argc != 2)
        return (write(1, "\n", 1), 0);
        
    str = argv[1];
    while (*str)
    {
        c = *str;
        if (c & 0x40)
        {
            // Apply ROT13 with bitwise modulo
            // (c - 'A' + 13) % 26
            if (c >= 'A' && c <= 'Z')
                c = ((c - 'A' + 13) & 0x1F) + 'A';
            else if (c >= 'a' && c <= 'z')  // Fixed: added 'if' after else
                c = ((c - 'a' + 13) & 0x1F) + 'a';  // Fixed: added missing parenthesis
        }
        write(1, &c, 1);
        str++;
    }
    write(1, "\n", 1);
    return (0);
}
