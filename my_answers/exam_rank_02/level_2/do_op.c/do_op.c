#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define WRITE_CHAR(c) (write(1,(c),1))

/**
 * ADDITION FIELD
 */
int ft_add(int a, int b)
{
    int carry;
    while (b != 0)
    {
        carry = a & b;
        a ^= b;
        b = carry << 1;
    }
    return (a);
}

int ft_add_recursive(int a, int b)
{
    if(b == 0)
        return (a);
    return ft_add_recursive(a ^ b, (a & b) << 1);
}

/**
 * MULTIPLICATION FIELD
 */
int ft_multiply(int a, int b)
{
    int result = 0;
    while(b != 0)
    {
        if (b & 1)
            result += a;
        a <<= 1;
        b >>= 1;
    }
    return (result);
}

int ft_multiply_recursive(int a, int b)
{
    if(b == 0)
        return 0;
    if (b & 1)
        return ft_add(a, ft_multiply_recursive(a << 1, b >> 1));
    else
        return ft_multiply_recursive(a << 1, b >> 1);
}

/**
 * SUBSTRACTION FIELD
 */
int ft_subtract(int a, int b)
{
    return ft_add(a, (~b + 1));
}

int ft_subtract_verbose(int a, int b)
{
    int carry;
    b = (~b + 1);
    while(b != 0)
    {
        carry = a & b;
        a ^= b;
        b = carry << 1;
    }
    return (a);
}

int ft_subtract_recursive(int a, int b)
{
    // Only convert b to two's complement at the top level
    return ft_add_recursive(a, (~b + 1));
}

/**
 * METHOD
 */
int ft_abs(int a)
{
    return (a < 0 ? ~a + 1 : a);
}

/**
 * DIVISION FIELD AND METHODS
 */
int ft_divide(int a, int b)
{
    if (b == 0)
        return 0;
    int sign = ((a < 0) ^ (b < 0)) ? -1 : 1;
    unsigned int ua = ft_abs(a);
    unsigned int ub = ft_abs(b);
    unsigned int quotient = 0;
    for (int i = 31; i >= 0; --i)
    {
        if ((ub << i) <= ua && (ub << i) > 0)
        {
            ua = ft_subtract(ua, ub << i);
            quotient |= (1U << i);
        }
    }
    return sign * (int)quotient;
}

int ft_divide_recursive_positive(unsigned int ua, unsigned int ub, int i)
{
    if (i < 0)
        return 0;
    if ((ub << i) > ua || (ub << i) == 0)
        return ft_divide_recursive_positive(ua, ub, i - 1);
    return (1U << i) | ft_divide_recursive_positive(ua - (ub << i), ub, i - 1);
}

int ft_divide_recursive(int a, int b)
{
    if (b == 0)
        return 0;
    int sign = ((a < 0) ^ (b < 0)) ? -1 : 1;
    unsigned int ua = ft_abs(a);
    unsigned int ub = ft_abs(b);
    int quotient = ft_divide_recursive_positive(ua, ub, 31);
    return sign * quotient;
}

int ft_divide2(int a, int b)
{
    int result = 0;
    while (a >= b)
    {
        int temp = b, multiple = 1;
        while (a >= (temp << 1))
        {
            temp <<= 1;
            multiple <<= 1;
        }
        a = ft_subtract(a, temp);
        result = ft_add(result, multiple);
    }
    return (result);
}

/**
 * MODULUS FIELD
 */
int ft_modulus_for_power_of2(int a, int b)
{
    return a & (b - 1);
}

int ft_modulus(int a, int b)
{
    if (b == 0)
        return 0;
    int sign = (a < 0) ? -1 : 1;
    unsigned int ua = ft_abs(a);
    unsigned int ub = ft_abs(b);
    while(ua >= ub)
        ua = ft_subtract(ua, ub);
    return sign * ua;
}

int ft_modulus_2(int a, int b)
{
    while(a >= b)
    {
        int temp = b;
        while(a >= (temp << 1))
            temp <<= 1;
        a = ft_subtract(a, temp);
    }
    return a;
}
//OTHER WAY TO DO IT WITHOUT BITWISE OPERATOR
/*
long long divide(long long a, long long b) {
    if(a == 0)
        return 0;
  
    // The sign will be negative only if sign of 
    // divisor and dividend are different
    int sign = ((a < 0) ^ (b < 0)) ? -1 : 1;
  
    // Convert divisor and dividend to positive numbers
    a = abs(a);
    b = abs(b);
  
    // Initialize the quotient
    long long quotient = 0;
  
    // Perform repeated subtraction till dividend
    // is greater than divisor
    while (a >= b) {
        a -= b;
        ++quotient;
    }
    return quotient * sign;
}
    */
typedef int (*operator_func)(int, int);

int ft_invalid(int a, int b)
{
    (void)a;
    (void)b;
    return 0;
}

int calcul_method(int num1, char op, int num2)
{
    static operator_func operators[128] = {NULL};
    static int initialized = 0;

    if (!initialized)
    {
        operators['+'] = ft_add;
        operators['-'] = ft_subtract;
        operators['*'] = ft_multiply;
        operators['/'] = ft_divide;
        operators['%'] = ft_modulus;
        initialized = 1;
    }
    return operators[(unsigned char)op] ? operators[(unsigned char)op](num1, num2) : ft_invalid(num1, num2);
}


int main(int argc, char **argv)
{
    int number1;
    char operation;
    int number2;
    if(argc != 4)
        return (WRITE_CHAR("\n"), 1);
    number1 = atoi(argv[1]);
    operation = argv[2][0];
    number2 = atoi(argv[3]);
    printf( "add iterative way: %d\n"
            "add recursive way: %d\n"
            "multiply iterative way: %d\n"
            "multiply recursive way: %d\n"
            "substract_verbose iterative way: %d\n"
            "callback substract iterative way: %d\n"
            "substract recursive way: %d\n"
            "divide iterative way: %d\n"
            "divide recursive way: %d\n"
            "divide iterative way other version: %d\n"
            "modulus iterative way: %d\n"
            "modulus other iterative way: %d\n",
            //"iterative way: %d\n",
            //"recursive way: %d\n", 
            ft_add(number1, number2),
            ft_add_recursive(number1, number2),
            ft_multiply(number1, number2),
            ft_multiply_recursive(number1, number2),
            ft_subtract_verbose(number1, number2),
            ft_subtract(number1, number2),
            ft_subtract_recursive(number1, number2),
            ft_divide(number1, number2),
            ft_divide_recursive(number1, number2),
            ft_divide2(number1, number2),
            ft_modulus(number1, number2),
            ft_modulus_2(number1, number2)
            //ft_modulus_recursive(number1, number2)
        );
    printf("==========================\n\n\nMETHOD OF CALCULATION\n\n\n==========================");
    printf("the calculr %d %c %d == %d\n",number1, operation, number2, calcul_method(number1,operation, number2));
    return(0);
}
