#include <unistd.h>
# define WRITE_NEW_LINE() (write(1,"\n",1))
// Only bitwise for case detection, no range checks
# define ISUPPER(c) (((c) & 0x40) && !((c) & 0x20))
# define ISLOWER(c) (((c) & 0x40) && ((c) & 0x20))
# define ROT13(c, base, rot) (((c - base + rot) % 26) + base)

void rot_13(char *str)
{
    char ch;
    while(*str)
    {
        ch = *str;
        if (ISUPPER(*str))
            ch = ROT13(*str, 'A', 1);
        else if (ISLOWER(*str))
            ch = ROT13(*str, 'a', 1);
        write(1, &ch, 1);
        str++;
    }
    WRITE_NEW_LINE();
}

int main(int argc, char **argv)
{
    char *str;
    if (argc != 2)
        return (WRITE_NEW_LINE(), 1);
    str = argv[1];
    rot_13(str);
    return (0);
}
