#include "get_next_line.h"

void    *ft_memmove(void *dst, const void *src, size_t n)
{
	const unsigned char *s = (const unsigned char *)src;
	unsigned char *d;

	d = (unsigned char*)dst;
	if (d == s)
		return (dst);
	if (d < s)
	{
		while (n--)
			*d++ = *s++;
	}
	else
	{
		d += n;
		s += n;
		while (n--)
			*--d = *--s;
	}
	return (dst);
}

void    *ft_memchr(void *ptr, int c, size_t n)
{
	unsigned char *p;

	p = (unsigned char*)ptr;
	while (n--)
	{
		if (*p == (unsigned char)c)
			return (p);
		p++;
	}
	return (NULL);
}

void    *ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new;
	size_t	copy_size;

	if (!new_size)
		return (NULL);
	new = malloc(new_size);
	if (!new)
		return (NULL);
	if (ptr && old_size)
	{
		if (old_size < new_size)
			copy_size = old_size;
		else
			copy_size = new_size;
		ft_memmove(new, ptr, copy_size);
		free(ptr);
	}
	return (new);
}
