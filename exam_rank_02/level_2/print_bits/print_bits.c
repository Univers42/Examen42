#include <unistd.h>

void print_bits(unsigned char octet)
{
    char bits[8];
    for (int i = 7; i >= 0; i--)
        bits[7 - i] = '0' + ((octet >> i) & 1);
    write(1,bits,8);
}

int main(void)
{
    unsigned char octet = 42;
    print_bits(octet);
    write(1, "\n", 1);
    return (0);
}