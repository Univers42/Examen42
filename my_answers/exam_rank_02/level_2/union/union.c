#include <unistd.h>

int main(int argc, char **argv)
{
    unsigned int seen[8] = {0};
    unsigned char c;
    char *str;
    int array_index;
    int bit_position;

    if (argc != 3)
        return(write(1, "\n", 1),0);
    for (int arg = 1; arg < argc; arg++)
    {
        str = argv[arg];
        while (*str)
        {
            c = *str;
            array_index = c >> 5;       // Divide by 32 to get the index
            bit_position = c & 0x1F;    //Modulo 32 (c % 32) to get the bit position
            if (!(seen[array_index] & (1 << bit_position)))
            {
                seen[array_index] |= (1 << bit_position);
                write(1, str, 1);
            }
            str++;
        }
    }
    write(1, "\n", 1);
    return (0);
}