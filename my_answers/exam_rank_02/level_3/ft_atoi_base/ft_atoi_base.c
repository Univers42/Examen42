#include <stdlib.h>
#include <stdio.h>

# define GREEN   "\033[32m"
# define RED     "\033[31m"
# define RESET   "\033[0m"
# define NEWLINE() (write(1, "\n", 1))
# define ISSPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\v' || (c) == '\f')
# define ISDIGIT(c) ((((c) & 0xF0) == 0x30) && (((c) & 0x0F) <= 9))
#define ISLOWER_HEX(c) ((c) >= 'a' && (c) <= 'f')
#define ISUPPER_HEX(c) ((c) >= 'A' && (c) <= 'F')
# define CONV_10(c) ((c) - '0')
# define CONV_HEX(c, base) (c - base + 10)


int conv_char(char c)
{
    if(ISDIGIT(c))
        return (CONV_10(c));
    else if(ISLOWER_HEX(c))
        return (CONV_HEX(c, 'a'));
    else if(ISUPPER_HEX(c))
        return (CONV_HEX(c, 'A'));
    else 
        return -1;
}

int ft_add(int a, int b)
{
    int carry;
    while( b != 0)
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
    if(b < 0)
    {
        b = -b;
        a = -a;
    }
    while(b != 0)
    {
        if (b & 1)
            result = ft_add(result, a);
        a <<= 1;
        b >>= 1;
    }
    return (result);
}

int ft_atoi_base(const char *str, int str_base)
{
    int res = 0;
    int sign = 0;
    int digit;

    if (str_base < 2 || str_base > 16)
        return (0);
    while(ISSPACE(*str))
        str++;
    if((*str ^ '+') == 0 || (*str ^ '-') == 0)
        sign = (*str++ ^ '-') == 0;
    while (*str)
    {
        digit = conv_char(*str);
        if (digit < 0 || digit >= str_base)
            break;
        res = ft_add(ft_multiply(res, str_base), digit);
        str++;
    }
    return(((~res + 1) & -sign) | (res & ~-sign));
}

int main(void)
{
    const char *tests[][2] = {
        {"101", "2"},
        {"77", "8"},
        {"123", "10"},
        {"7B", "16"},
        {"7b", "16"},
        {"-1A", "16"},
        {"255", "16"},
        {"fff", "16"},
        {"-42", "10"},
        {"42", "4"},
        {"-101", "2"},
        {"", "10"},
        {"g", "16"},
        {"123", "5"},
        {"   123", "10"},
        {"  -1a", "16"},
        {"0", "10"},
        {"-0", "10"},
        {"0001", "2"},
        {"-0001", "2"},
        {"--42", "10"},
        {"+42", "10"},
        {"-+42", "10"},
        {"42abc", "10"},
        {"abc42", "16"},
        {"12fcB3 ", "16"},
        {"1G", "16"},
        {"1", "1"},
        {NULL, NULL}
    };

    for (int i = 0; tests[i][0]; i++) {
        const char *str = tests[i][0];
        int base = atoi(tests[i][1]);
        int std = (int)strtol(str, NULL, base);
        int mine = ft_atoi_base(str, base);
        int ok = (std == mine);
        printf("str=\"%s\" base=%d | strtol: %d | ft_atoi_base: %d | %s%s%s\n",
            str, base, std, mine,
            ok ? GREEN : RED,
            ok ? "OK" : "DIFF",
            RESET);
    }
    return 0;
}
