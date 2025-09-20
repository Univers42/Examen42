/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   training.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 20:10:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 17:23:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef DYN_STR
# define DYN_STR 1
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif
#endif

int current_removal = 0;
int current_balance = 0;
int min_rem = 0;

int	ft_strlen(char *str)
{
	char *tmp;

	tmp = str;
	while (*tmp++);
	return (tmp - str - 1);
}

void	make_move(char *seq, int *a, int k, int value) {
    // Update removal count and balance based on decision
    if ((seq[k] == '(' || seq[k] == ')') && value == 0)
        current_removal++;
    else if (value == 1) {
        if (seq[k] == '(')
            current_balance++;
        else if (seq[k] == ')')
            current_balance--;
    }
    a[k] = value;
}

void	unmake_move(char *seq, int *a, int k, int value) {
    // Revert the effects of make_move
	(void)a;
    if ((seq[k] == '(' || seq[k] == ')') && value == 0)
        current_removal--;
    else if (value == 1) {
        if (seq[k] == '(')
            current_balance--;
        else if (seq[k] == ')')
            current_balance++;
    }
    // leave a[k] as-is; it will be overwritten on the next decision
}

void print_solution(char *seq, int *a, int n) {
    for (int i = 0; i < n; i++) {
        if ((seq[i] == '(' || seq[i] == ')') && a[i] == 0)
            write(1, "_", 1);
        else
            write(1, &seq[i], 1);
        write(1, " ", 1); // always print a space after each character
    }
    write(1, "\n", 1);
}

void	build_candidate(char *seq, int k, int *candidate, int *nc) {
    if (seq[k] == '(' || seq[k] == ')') {
        candidate[0] = 1; // keep
        candidate[1] = 0; // remove
        *nc = 2;
    } else {
        candidate[0] = 1;
        *nc = 1;
    }
}

void	rip(char *seq, int *a, int k, int n)
{
	int candidate[2];
	int nc;
	int i;

    // Prune: too many removals or invalid balance
    if (current_removal > min_rem || current_balance < 0)
		return;
	if (k == n) {
        if (current_removal == min_rem && current_balance == 0)
            print_solution(seq, a, n);
        return;
    }
    build_candidate(seq, k, candidate, &nc);
    for (i = 0; i < nc; i++) {
        make_move(seq, a, k, candidate[i]);
        rip(seq, a, k + 1, n);
        unmake_move(seq, a, k, candidate[i]);
    }
}

void append_char(char *buf, int *len, char c) {
    buf[*len] = c;
    (*len)++;
}

// Preprocess input: remove spaces, keep only '(' and ')'
char *preprocess_input(const char *input, int *out_len) {
    int len = 0;
    int input_len = strlen(input);
    char *buf = malloc(input_len + 1);
    if (!buf)
        return NULL;
    for (int i = 0; input[i]; i++) {
        if (input[i] == '(' || input[i] == ')')
            append_char(buf, &len, input[i]);
    }
    buf[len] = '\0';
    *out_len = len;
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2)
        return 1;
    int n;
    char *seq = preprocess_input(argv[1], &n);
    if (!seq)
        return 1;
    int left = 0, right = 0;
    for (int i = 0; seq[i]; i++) {
        if (seq[i] == '(')
            left++;
        else if (seq[i] == ')') {
            if (left > 0)
                left--;
            else
                right++;
        }
    }
    min_rem = left + right;
    int *a = calloc(n, sizeof(int));
    rip(seq, a, 0, n);
    free(a);
    free(seq);
    return 0;
}