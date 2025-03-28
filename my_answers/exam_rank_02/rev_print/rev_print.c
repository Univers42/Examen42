#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        write(1, "\n", 1);
        return (0);
    }
    
    char *str = argv[1];
    int len = 0;
    
    // Find string length - fastest approach
    while (str[len])
        len++;
    
    // Reverse print in a single loop with pre-decrement
    while (len--)
        write(1, &str[len], 1);
    
    write(1, "\n", 1);
    return (0);
}
