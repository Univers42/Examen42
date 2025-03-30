// Stein's algorithm for computing the lcm of two integers
#include <stdio.h>
unsigned int gcd(unsigned int a, unsigned int b)
{
    if (a == b) return a;
    if (a == 0) return b;
    if (b == 0) return a;
    int shift = 0;
    while(((a | b) & 1) == 0)
    {
        a >>= 1;
        b >>= 1;
        shift++;
    }
    while((a & 1) == 0)
        a >>= 1;
    do {
        while((b & 1) == 0)
            b >>= 1;
        if (a > b)
        {
            unsigned int t = b;
            b = a;
            a = t;
        }
        b -= a;
    }while(b != 0);
    return a << shift;
}

unsigned int lcm(unsigned int a, unsigned int b)
{
    if (a == 0 || b == 0) return 0;
    if (a == b) return a;
    return (a / gcd(a, b)) * b;
}


int main(void)
{
    unsigned int test_cases[][2] = {
        {12, 18},  // LCM(12, 18) = 36
        {7, 5},    // LCM(7, 5) = 35
        {0, 10},   // LCM(0, 10) = 0
        {15, 20},  // LCM(15, 20) = 60
        {1, 1},    // LCM(1, 1) = 1
        {100, 250} // LCM(100, 250) = 500
    };

    for (int i = 0; i < 6; i++)
    {
        unsigned int a = test_cases[i][0];
        unsigned int b = test_cases[i][1];
        printf("LCM(%u, %u) = %u\n", a, b, lcm(a, b));
    }

    return 0;
}
