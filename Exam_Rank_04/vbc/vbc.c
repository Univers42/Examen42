/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 22:27:43 by dlesieur          #+#    #+#             */
/*   Updated: 2026/02/23 22:27:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <stdlib.h>

static char *s;

void error(char c) {
	c ? printf("Unexpected token '%c'\n", c) : printf("Unexpected end of input\n");
	exit(1);
}

long long add(long long a, long long b) { return a + b; }
long long mul(long long a, long long b) { return a * b; }

long long parse_factor(void);
long long parse_term(void);

long long parse_op(char op, long long (*f)(long long, long long), long long (*sub)(void)) {
	long long res = sub();
	while (*s == op) s++, res = f(res, sub());
	return res;
}

long long parse_factor(void) {
	long long res;
	if (*s == '(')
		return s++, res = parse_op('+', add, parse_term), *s == ')' ? (s++, res) : (error(*s), 0LL);
	if (*s >= '0' && *s <= '9')
		return *s++ - '0';
	return error(*s), 0LL;
}

long long parse_term(void) { return parse_op('*', mul, parse_factor); }

int main(int ac, char **av) {
	if (ac != 2) return 0;
	s = av[1];
	long long res = parse_op('+', add, parse_term);
	if (*s) error(*s);
	printf("%lld\n", res);
}