#include <unistd.h>

// Function to calculate string length
int ft_strlen(char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return(write(1, "\n", 1), 0);

    char *s1 = argv[1]; // String to match
    char *s2 = argv[2]; // Source of characters
    int i = 0;          // Index for s1
    int j = 0;          // Index for s2
    
    while (s1[i] && s2[j])
    {
        if (s1[i] == s2[j])
            i++;
        j++;
    }
    if (!s1[i])
        write(1, s1, ft_strlen(s1));
    write(1, "\n", 1);
    return 0;
}