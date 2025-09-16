/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_scanf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:38:08 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/15 20:01:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER_SIZE 10000

int match_space(FILE *f)
{
	int c;

	c = fgetc(f);
	if (isspace(c))
	{
		while (isspace(c))
			c = fgetc(f);
		ungetc(c, f);
		return (1);
	}
	ungetc(c, f);
	return (0);
}

int match_char(FILE *f, char c)
{
	int match;

	match = fgetc(f);
	if (match == c)
		return (1);
	ungetc(c, f);
	return (0);
}

int scan_char(FILE *f, va_list ap)
{
	char *ptr;
	char c;

	ptr = va_arg(ap, char *);
	c = fgetc(f);
	if (c == EOF)
		return (0);
	*ptr = (char)c;
	return (1);
}

void	convert_number(char *buffer, int *ptr)
{
	int		digit;
	bool	is_negative;
	int		result;

	is_negative = false;
	result = 0;
	digit = 0;
	if (*buffer == '-' || *buffer == '+')
	{
		if (*buffer == '-')
			is_negative = true;
		buffer++;
	}
	while(isdigit((unsigned char)*buffer))
	{
		digit = (*buffer - '0');
		result = result * 10 + digit;
		buffer++;
	}
	result = is_negative ? -result : result;
	*ptr = result;
}

int scan_int(FILE *f, va_list ap)
{
	char buffer[32];
	int i;
	char c;
	int *ptr;

	ptr = va_arg(ap, int*);
	c = fgetc(f);
	i = 0;
	if (c == '-' || c == '+')
		buffer[i++] = c;
	else
		ungetc(c, f);
	c = fgetc(f);
	while (c != EOF && isdigit(c) && i < 31)
	{
		buffer[i++] = c;
		c = fgetc(f);
	}
	if (c != EOF && !isdigit(c))
		ungetc(c, f);
	buffer[i] = '\0';
	convert_number(buffer, ptr);
	return (1);
}

int scan_string(FILE *f, va_list ap)
{
    char buf[1024];
    int	i;
    int	c;
    char *ptr;

    i = 0;
    ptr = va_arg(ap, char *);
    c = fgetc(f);
    while (c != EOF && !isspace(c) && i < 1023)
    {
        buf[i++] = c;
        c = fgetc(f);
    }
    if (c != EOF && isspace(c))
        ungetc(c, f);
    buf[i] = '\0';
	i = 0;
    while((ptr[i] = buf[i]))
        i++;
    return (i > 0 ? 1 : 0);
}

int	match_conv(FILE *f, const char **format, va_list ap)
{
	switch (**format)
	{
		case 'c':
			return scan_char(f, ap);
		case 'd':
			match_space(f);
			return scan_int(f, ap);
		case 's':
			match_space(f);
			return scan_string(f, ap);
		case EOF:
			return -1;
		default:
			return -1;
	}
}

int ft_vfscanf(FILE *f, const char *format, va_list ap)
{
	int nconv = 0;

	int c = fgetc(f);
	if (c == EOF)
		return EOF;
	ungetc(c, f);

	while (*format)
	{
		if (*format == '%')
		{
			format++;
			if (match_conv(f, &format, ap) != 1)
				break;
			else
				nconv++;
		}
		else if (isspace(*format))
		{
			if (match_space(f) == -1)
				break;
		}
		else if (match_char(f, *format) != 1)
			break;
		format++;
	}
	
	if (ferror(f))
		return EOF;
	return nconv;
}

int ft_scanf(const char *fmt, ...)
{
	va_list ap;
	int scanned;

	va_start(ap, fmt);
	scanned = ft_vfscanf(stdin, fmt, ap);   
	va_end(ap);
	return (scanned);
}

int main(void)
{
    char str[1024];
    int num;
    char ch;

	printf("Wite a character: ");
	ft_scanf("%c", &ch);
	printf("You hit : %c\n", ch);
    printf("write a string: ");
    ft_scanf("%s", str);
    printf("You wrote: %s\n", str);

    printf("write an integer: ");
    ft_scanf("%d", &num);
    printf("You wrote: %d\n", num);

    printf("write a character: ");
    ft_scanf("%c", &ch);
    printf("You wrote: %c\n", ch);

    printf("write a string and an integer: ");
    ft_scanf("%s %d", str, &num);
    printf("You wrote: %s and %d\n", str, num);

    return 0;
}