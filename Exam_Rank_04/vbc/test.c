/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 22:49:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/23 23:05:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *s;

void    error(char c)
{
    c ? printf("Unexpected token '%c'\n", c) : printf("Unexpected end of input\n");
    exit(1);
}

long long apply(char op, long long a, long long b)
{
    if (op == '+')
        return (a + b);
    else if (op == '-')
        return (a - b);
    else if (op == '*')
        return (a * b);
    else if (op == '/' || op == '%')
    {
        if (b == 0)
        {
            printf("Division by zero\n");
            exit(1);
        }
        return (op == '/' ? a / b : a % b);
    }
    return (0);
}

long long parse_factor(void);
long long parse_term(void);

long long parse_op(const char *ops, long long (*sub)(void))
{
    long long   res;
    char        op;

    res = sub();
    while ((op = *s) && strchr(ops, op))
    {
        s++;
        res = apply(op, res, sub());
    }
    return (res);
}

long long parse_factor(void)
{
    long long   res;

    if (*s == '-')
        return (s++, -parse_factor());
    if (*s == '+')
        return (s++, parse_factor());
    if (*s == '(')
    {
        s++;
        res = parse_op("+-", parse_term);
        return ((*s == ')') ? (s++, res) : (error(*s), 0LL));
    }
    if (*s >= '0' && *s <= '9')
        return (*s++ - '0');
    return (error(*s), 0LL);
}

long long parse_term(void) { return parse_op("*/%", parse_factor); }

int main(int argc, char **argv)
{
    long long   res;

    if (argc != 2)
        return (0);
    s = argv[1];
    res = parse_op("+-", parse_term);
    if (*s)
        error(*s);
    printf("%lld\n", res);
}