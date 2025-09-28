#include <unistd.h>
# define NEWLINE() (write(1,"\n",1))

void print_hex_iterative(int number)
{
    char hex[] = "0123456789abcdef";
    char buffer[16];
    int  i = 0;

    if (number == 0)
    {
        write(1,"0",1);
        return ;
    }
    while(number > 0)
    {
        buffer[i++] = hex[number % 16];
        number /= 16;  
    }
    while(i--)
        write(1,&buffer[i],1);
}

void print_hex(int number)
{
    char hex[] = "0123456789abcdef";
    if (number >= 16)
        print_hex(number >> 4); // print_hex(number / 16)
    write(1, &hex[number & 0xF], 1); //hex[number % 16]
}

int main(void)
{
    int number = 67;
    print_hex(number);
    NEWLINE();
    return (0);
}
