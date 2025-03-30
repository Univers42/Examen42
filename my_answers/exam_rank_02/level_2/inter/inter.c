#include <unistd.h>

int main(int argc, char **argv)
{
    unsigned char s2_chars[32] = {0};
    unsigned char used[32] = {0};

    if (argc != 3)
        return (write(1, "\n", 1), 0);
    char *s1 = argv[1];
    char *s2 = argv[2];
    while(*s2)
    {
        s2_chars[*s2 >> 3] |= (1 << (*s2 & 7));
        s2++;
    }
    while(*s1)
    {
        if ((s2_chars[*s1 >> 3] & (1 << (*s1 & 7))) && 
            !(used[*s1 >> 3] & (1 << (*s1 & 7))))
            {
                write(1, s1, 1);
                used[*s1 >> 3] |= (1 << (*s1 & 7));
            }
            s1++;
    }
    write(1, "\n", 1);
    return (0);
}