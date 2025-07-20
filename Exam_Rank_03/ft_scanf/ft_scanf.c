#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

int match_space(FILE *f)
{
	int c;
	int matched = 0;
	
	while ((c = fgetc(f)) != EOF && isspace(c))
		matched = 1;
	
	if (c == EOF)
		return matched ? 0 : -1;
	
	ungetc(c, f);
	return 0;
}

int match_char(FILE *f, char target)
{
	int c = fgetc(f);
	
	if (c == EOF)
		return -1;
	
	if (c == target)
		return 1;
	
	ungetc(c, f);
	return 0;
}

int scan_char(FILE *f, va_list ap)
{
	int c = fgetc(f);
	
	if (c == EOF)
		return -1;
	
	char *ptr = va_arg(ap, char*);
	*ptr = (char)c;
	return 1;
}

int scan_int(FILE *f, va_list ap)
{
	int c;
	int sign = 1;
	int value = 0;
	int digits_read = 0;
	
	// Read first character
	c = fgetc(f);
	if (c == EOF)
		return -1;
	
	// Handle optional sign
	if (c == '-') {
		sign = -1;
		c = fgetc(f);
	} else if (c == '+') {
		c = fgetc(f);
	}
	
	// Check if we have at least one digit
	if (c == EOF || !isdigit(c)) {
		if (c != EOF)
			ungetc(c, f);
		return -1;
	}
	
	// Read digits
	while (c != EOF && isdigit(c)) {
		value = value * 10 + (c - '0');
		digits_read = 1;
		c = fgetc(f);
	}
	
	// Put back the non-digit character
	if (c != EOF)
		ungetc(c, f);
	
	if (!digits_read)
		return -1;
	
	int *ptr = va_arg(ap, int*);
	*ptr = value * sign;
	return 1;
}

int scan_string(FILE *f, va_list ap)
{
	int c;
	char *str = va_arg(ap, char*);
	int chars_read = 0;
	
	// Read first non-whitespace character
	c = fgetc(f);
	if (c == EOF)
		return -1;
	
	// If it's whitespace, we have no string to read
	if (isspace(c)) {
		ungetc(c, f);
		return -1;
	}
	
	// Read non-whitespace characters
	while (c != EOF && !isspace(c)) {
		*str++ = (char)c;
		chars_read = 1;
		c = fgetc(f);
	}
	
	// Put back the whitespace or EOF
	if (c != EOF)
		ungetc(c, f);
	
	if (!chars_read)
		return -1;
	
	*str = '\0';
	return 1;
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