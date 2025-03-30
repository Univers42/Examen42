#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef int(*op_func)(int, int);

/**
 * Efficient jump table :abort
 * O(1) lookup table for operations
 * - eliminate branching overhead
 * Function to add two integers using bitwise operations
 * This function uses bitwise AND, XOR, and left shift to perform addition
 * without using the + operator.
 * 
 * @param a First integer
 * @param b Second integer
 * @return The sum of a and b
 */

 /**
  * Function to add two integers using bitwise operations
  * This function uses bitwise AND, XOR, and left shift to perform addition
  * without using the + operator.
  * The function works by breaking addition into two parts: 
  * Addign bits without carrying using (XOR)
  * and carrying bits using (AND) and left shifting them.
  * Examples : 
  * Initial values: a = 5 (binary : 0101), b = 3 (binary : 0011)
  * 1. Carry = a & b = 0101 & 0011 = 0001 (carry)
  * 2. a = a ^ b = 0101 ^ 0011 = 0110 (sum without carry)
  * 3. b = carry << 1 = 0001 << 1 = 0010 (shift carry left)
  * Iteration 2 
  * carry = a &  b = 0110 & 0010 = 0010 (carry)
  * a = a ^ b = 0110 ^ 0010 = 0100 (sum without carry)
  * b = carry << 1 = 0100 << 1 = 1000 (shift carry left)
  * Iteration 3
  * carr = a & b = 0000 & 1000 = 0000 ( no carries )
  * a = a ^ b = 0000 ^ 1000 = 1000 (sum without carry)
  * b = carry << 1 = 0000 << 1 = 0000 (shift carry left)
  * 
  * since b is now 0, the loop terminates and a = 8 ( binary : 1000)
  */
int add(int a, int b)
{
    while (b != 0)
    {
        int carry = a & b;
        a = a ^ b;
        b = carry << 1;
    }
    return a;
}
/**
 * for 9 - 5:
 * 1. convert to : 9 + (-5)
 * 2. find -5 using two's complement: ~5 + 1
 * ~5 = ~(0000 0101) = 1111 1010
 * ~5 + 1 = 11111011 (which is -5 in two's complement)
 * 3. Calculate 9 + (-5) using our add function
 * result : 4
 */
int substract(int a, int b)
{
    return add(a, ~b + 1);
}
/**
 * 7 * 5
 * Inital values : 
 * a = 7 (binary : 0111)
 * b = 5 (binary : 0101)
 * result = 0
 * sign = 1
 * 
 * Iteration 1: 
 * b & 1 = 0101 & 0001 = 0001 (true)
 * result = add(0, 7) = 0111 (result)
 * a = 7 << 1 = 14 (binary : 1110)
 * b = 5 >> 1 = 2 (binary : 0010)
 * 
 * Iteration 2:
 * b & 1 = 0010 & 0001 = 0000 (false)
 * a = 14 << 1 = 28 (binary : 11100)
 * b = 2 >> 1 = 1 (binary : 0001)
 * 
 * Iteration 3:
 * b & 1 = 0001 & 0001 = 0001 (true)
 * result = add(0111, 11100) = 10011 (result)
 * a = 28 << 1 = 56 (binary : 111000)
 * b = 1 >> 1 = 0 (binary : 0000)
 * 
 * b is now 0, so we exit the loop. 
 * sign is 1, so we return result = 35
 */
int multiply(int a, int b)
{
    int result = 0;
    int sign = 1;

    if (a < 0)
    {
        a = ~a + 1;
        sign = -sign;
    }
    if (b < 0)
    {
        b = ~b + 1;
        sign = -sign;
    }
    while (b != 0)
    {
        if (b & 1)
            result = add(result, a);
        a <<= 1;
        b >>= 1;
    }
    return sign == 1 ? result : ~result + 1;
}

int divide(int a, int b)
{
    return b != 0 ? a / b : 0;
}

int modulo (int a, int b)
{
    return b != 0 ? a % b : 0;
}

int main(int argc, char **argv)
{
    if (argc != 4) 
        return (write(1, "\n", 1), 0);
    int num1 = atoi(argv[1]);
    char op = argv[2][0];
    int num2 = atoi(argv[3]);
    int result = 0;

    // jump_table to map oprators to functions
    op_func operations[128] = {0};
    operations['+'] = add;
    operations['-'] = substract;
    operations['*'] = multiply;
    operations['/'] = divide;
    operations['%'] = modulo;

    if (operations[(int)op])
        result = operations[(int)op](num1, num2);
    printf("%d\n", result);
    return (0);
}