#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *s;

void error(char c) {
    c ? printf("Unexpected token '%c'\n", c) : printf("Unexpected end of input\n");
    exit(1);
}

long long apply(char op, long long a, long long b) {
    if (op == '+') return (a + b);
    if (op == '-') return (a - b);
    if (op == '*') return (a * b);
    if (op == '/' || op == '%')
	{
        if (b == 0) { printf("Division by zero\n"); exit(1); }
        return op == '/' ? a / b : a % b;
    }
    return 0;
}

long long parse_factor(void);
long long parse_term(void);

long long parse_op(const char *ops, long long (*sub)(void)) {
    long long res = sub();
    char op;
    while ((op = *s) && strchr(ops, op)) s++, res = apply(op, res, sub());
    return res;
}

long long parse_factor(void) {
    if (*s == '-') return s++, -parse_factor();
    if (*s == '+') return s++,  parse_factor();
    if (*s == '(') {
        s++;
        long long res = parse_op("+-", parse_term);
        return *s == ')' ? (s++, res) : (error(*s), 0LL);
    }
    if (*s >= '0' && *s <= '9') return *s++ - '0';
    return error(*s), 0LL;
}

long long parse_term(void) { return parse_op("*/%", parse_factor); }

int main(int ac, char **av) {
    if (ac != 2) return 0;
    s = av[1];
    long long res = parse_op("+-", parse_term);
    if (*s) error(*s);
    printf("%lld\n", res);
}
