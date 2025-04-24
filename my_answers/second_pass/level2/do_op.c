#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================= UTILS FOR EXERCICE ==============================

# define WRITE_CHAR(x, size) (write(1, &(x), size))
# define NEWLINE() (WRITE_CHAR("\n", 1))
# define IS_SPACE(x) ((x ^ ' ') == 0 || (x ^'\t') == 0 || (x ^'\n') == 0)
# define IS_PRINTABLE(x) (x >= 32 && x <= 126)
# define IS_UPPER(x) (((x) & ~0x1F) == 0x40)
# define IS_LOWER(x) (((x) & ~0x1F) == 0x60)
# define IS_DIGIT(x) (((c & 0xF0) == 0x30) && ((c & 0x0F) <= 9))
# define IS_ALPHA(x) (IS_UPPER(x) || IS_LOWER(x))
# define IS_ALNUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

// ========================= SPECIFIC FOR EXERCICE ==============================

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

int ft_multiply(int a, int b)
{
    int result = 0;
    int negative = 0;

    if (a < 0)
    { 
        a = -a; 
        negative ^= 1;
    }
    if (b < 0)
    { 
        b = -b; 
        negative ^= 1;
    }

    while (b != 0)
    {
        if (b & 1)
            result = ft_add(result, a);
        a <<= 1;
        b >>= 1;
    }
    return negative ? -result : result;
}

int ft_subtract(int a, int b)
{
    return (ft_add(a, ft_add(~b, 1)));
}

// we still have to learn them divide and modulo
int ft_divide(int dividend, int divisor)
{
    if (divisor == 0)
        return 0;

    int sign = ((dividend < 0) ^ (divisor < 0)) ? -1 : 1;
    unsigned int a = (dividend < 0) ? -dividend : dividend;
    unsigned int b = (divisor < 0) ? -divisor : divisor;
    unsigned int result = 0;

    for (int i = 31; i >= 0; i--)
    {
        if ((a >> i) >= b)
        {
            a = ft_subtract(a, (b << i));
            result |= (1U << i);
        }
    }
    return sign * result;
}

int ft_modulo(int a, int b)
{
    if (b == 0)
        return 0;
    int sign = (a < 0) ? -1 : 1;
    unsigned int ua = (a < 0) ? -a : a;
    unsigned int ub = (b < 0) ? -b : b;
    for (int i = 31; i >= 0; i--)
    {
        if ((ua >> i) >= ub)
            ua = ft_subtract(ua, (ub << i));
    }
    return (sign * ua);
}

int do_op(int n1, char operation, int n2)
{
    typedef int (*ops)(int, int);
    char *operations = "+-*/%";
    ops op_math[5] = {
        ft_add,
        ft_subtract,
        ft_multiply,
        ft_divide,
        ft_modulo
    };

    for (int i = 0; i < 5; i++)
        if (operation == operations[i])
            return op_math[i](n1, n2);

    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        write(1, "\n", 1);
        return (1);
    }
    int n1 = atoi(argv[1]);
    char op = argv[2][0];
    int n2 = atoi(argv[3]);
    printf("%d %c %d = %d\n", n1, op, n2, do_op(n1, op, n2));
    return (0);
}
