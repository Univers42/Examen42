/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_scanf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/20 14:29:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

//Allowed functions: fgetc, ungetc, ferror, feof, isspace, isdigit, stdin, va_start, va_arg, va_copy, va_end
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
	int	c;

	ptr = va_arg(ap, char *);
	c = fgetc(f);
	if (c == EOF)
		return (0);
	*ptr = (char)c;
	return (1);
}

void convert(char *buf, int *ptr)
{
    int result = 0;
    int digit;
    int is_negative = 0;

    if (*buf == '-' || *buf == '+')
    {
        if (*buf == '-')
            is_negative = 1;
        buf++;
    }
    while (isdigit((unsigned char)*buf))
    {
        digit = (*buf - '0');
        result = result * 10 + digit;
        buf++;
    }
    if (is_negative)
        result = -result;
    *ptr = result;
}

int scan_int(FILE *f, va_list ap)
{
    char buf[32];
    int i = 0;
    int c;
    int *ptr;

    ptr = va_arg(ap, int*);
    c = fgetc(f);
    if (c == '-' || c == '+')
        buf[i++] = c;
    else
        ungetc(c, f);
    c = fgetc(f);
    while (c != EOF && isdigit(c) && i < 31)
    {
        buf[i++] = c;
        c = fgetc(f);
    }
    if (c != EOF && !isdigit(c))
        ungetc(c, f);
    buf[i] = '\0';
    convert(buf, ptr);
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


int ft_scanf(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = ft_vfscanf(stdin, format, ap);
	va_end(ap);
	return ret;
}

/**
The issue is that when you use scanf("%c", &ch) after scanf("%d", &num),
the previous input leaves a newline ('\n') character in the input buffer.
scanf("%c") reads this leftover newline, not a new character from the user.
This is a common behavior with scanf and character input.

How to fix:
Before reading a character with scanf("%c", &ch), we should consume
any leftover whitespace (especially '\n') in the input buffer.
We can do this by adding a space before %c: scanf(" %c", &ch);

*/
int main(void)
{
    char str[1024];
    int num;
    char ch;

    printf("write a string: ");
    scanf("%s", str);
    printf("You wrote: %s\n", str);

    printf("write an integer: ");
    scanf("%d", &num);
    printf("You wrote: %d\n", num);

    printf("write a character: ");
    // The space before %c tells scanf to skip any whitespace (like leftover newlines)
    // from previous input, so it reads the next non-whitespace character entered by the user.
    scanf(" %c", &ch);
    printf("You wrote: %c\n", ch);

    printf("write a string and an integer: ");
    scanf("%s %d", str, &num);
    printf("You wrote: %s and %d\n", str, num);

    return 0;
}