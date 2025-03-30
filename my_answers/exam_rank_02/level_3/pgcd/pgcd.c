#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//int pgcd(int a, int b)
//{
//    while (b)
//    {
//        int temp = b;
//        b = a % b;
//        a = temp;
//    }
//    return a;
//}
//
/**
 * if both numbers are even, factor out 2 using bitwise shit >> 
 * if one number is even, divide only the evem one by 2
 * Use binary substraction-based GCD (stein's algorithm instead of modulo)
 */
#include <stdio.h>
#include <stdlib.h>

// Correct implementation of GCD using binary algorithm
unsigned int pgcd(unsigned int a, unsigned int b)
{
    // Base cases
    if (a == b) return a;
    if (a == 0) return b;
    if (b == 0) return a;
    // Check if both numbers are even
    if ((a & 1) == 0 && (b & 1) == 0)
        return pgcd(a >> 1, b >> 1) << 1;
    // If only a is even
    if ((a & 1) == 0)
        return pgcd(a >> 1, b);
    // If only b is even
    if ((b & 1) == 0)
        return pgcd(a, b >> 1);
    // Both are odd, subtract smaller from larger
    if (a > b)
        return pgcd(a - b, b);
    else
        return pgcd(b - a, a);
}


int main(int argc, char **argv)
{
    if (argc == 3)
    {
        int a = atoi(argv[1]);
        int b = atoi(argv[2]);

        if (a > 0 && b > 0)
            printf("%d\n", pgcd(a, b));
        else
            printf("\n");
    }
    else
        printf("\n");    
    return 0;
}