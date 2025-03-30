#include <unistd.h>
//Direct count retrieval which is a constant time operation
void putnbr(int n)
{
    char c;
    
    if (n > 9)
        putnbr(n / 10);   // Normal division remains for recursion
    c = (n & 0b1111) + '0';  // Faster modulo (n % 10) using bitwise mask
    write(1, &c, 1);
}

int main(int argc, char **argv)
{
    (void)argv;
    putnbr(argc - 1);
    write(1, "\n", 1);
    return 0;
}