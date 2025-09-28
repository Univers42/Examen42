#include <unistd.h>

enum e_bitmap 
{
    ASCII_TABLE = 0xFF,
    NULL_TERMINATING_VALUE = 0x00
};

char *ft_strcpy(char *s1, char *s2)
{
    char *dest;
    
    dest = s1;    
    while(*s2 & ASCII_TABLE)
        *s1++ = (*s2++ & ASCII_TABLE);
    *s1 = NULL_TERMINATING_VALUE & ASCII_TABLE;
    return (dest);
}

int main()
{
    char source[] = "Hello, world!";
    char destination[20];
    
    ft_strcpy(destination, source);
    
    // Print the copied string
    char *ptr = destination;
    while (*ptr)
        write(1, ptr++, 1);
    write(1, "\n", 1);
    
    return 0;
}
