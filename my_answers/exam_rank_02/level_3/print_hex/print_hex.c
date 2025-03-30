#include <unistd.h>
//recursive approach O(log(n))

void print_hex(unsigned int n)
{
    char hex[] = "0123456789abcdef";
    if (n >= 16)
        print_hex(n >> 4); //Divide by 16 using bit shift
    write(1, &hex[n & 0xF], 1);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        unsigned int num = 0;
        for (int i = 0; argv[1][i]; i++)
        {
            if (argv[1][i] < '0' || argv[1][i] > '9')
            {
                write(1, "\n", 1);
                return (0);
            }
            num = (num << 3) + (num << 1) + (argv[1][i] - '0');
        }
        print_hex(num);
    }
    write(1, "\n", 1);
    return 0;
}