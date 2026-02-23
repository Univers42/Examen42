/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   complete_vbc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 22:43:29 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/23 22:44:58 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *s;

void error(char c) {
    c ? printf("Unexpected token '%c'\n", c) : printf("Unexpected end of input\n");
    exit(1);
}

long long parse_expr(void);

long long fact(long long n) {
    if (n < 0) { printf("Negative factorial\n"); exit(1); }
    long long r = 1; while (n > 1) r *= n--; return r;
}

long long ipow(long long b, long long e) {
    if (e < 0) { printf("Negative exponent\n"); exit(1); }
    long long r = 1; while (e--) r *= b; return r;
}

long long apply(char op, long long a, long long b) {
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '&') return a & b;
    if (op == '|') return a | b;
    if (op == '/' || op == '%') {
        if (!b) { printf("Division by zero\n"); exit(1); }
        return op == '/' ? a / b : a % b;
    }
    return 0;
}

long long parse_factor(void) {
    if (*s == '-') return s++, -parse_factor();
    if (*s == '+') return s++,  parse_factor();
    if (*s == '~') return s++, ~parse_factor();
    char close = *s == '(' ? ')' : *s == '[' ? ']' : 0;
    if (close) {
        s++;
        long long res = parse_expr();
        if (*s != close) error(*s ? *s : 0);
        s++;
        while (*s == '!') { s++; res = fact(res); }
        return res;
    }
    if (*s >= '0' && *s <= '9') {
        long long n = 0;
        while (*s >= '0' && *s <= '9') n = n * 10 + (*s++ - '0');
        while (*s == '!') { s++; n = fact(n); }
        return n;
    }
    return error(*s), 0LL;
}

long long parse_power(void) {
    long long res = parse_factor();
    if (*s == '^') { s++; return ipow(res, parse_power()); }
    return res;
}

long long parse_op(const char *ops, long long (*sub)(void)) {
    long long res = sub();
    char op;
    while ((op = *s) && strchr(ops, op)) s++, res = apply(op, res, sub());
    return res;
}

long long parse_term(void) { return parse_op("*/%", parse_power); }
long long parse_add(void)  { return parse_op("+-", parse_term); }
long long parse_bits(void) { return parse_op("&|", parse_add); }

long long parse_expr(void) {
    long long res = parse_bits();
    if (*s != '?') return res;
    s++;
    long long a = parse_expr();
    if (*s != ':') error(*s ? *s : 0);
    s++;
    long long b = parse_expr();
    return res ? a : b;
}

int main(int ac, char **av) {
    if (ac != 2) return 0;
    s = av[1];
    long long res = parse_expr();
    if (*s) error(*s);
    printf("%lld\n", res);
}