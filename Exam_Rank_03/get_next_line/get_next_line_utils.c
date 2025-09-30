#include "get_next_line.h"

void    *ft_memmove(void *dst, const void *src, size_t n)
{
	const unsigned char *s = (const unsigned char *)src;
	unsigned char *d;

	d = (unsigned char *)dst;
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
	unsigned char	*s;

	s = (unsigned char *)ptr;
	while (n--)
	{
		if (*s == (char)c)
			return (s);
		s++;
	}
	return (NULL);
}

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*tmp;
	size_t	copy_size;

	if (!new_size)
		return  (NULL);
	tmp = malloc(new_size);
	if (!tmp)
		return (tmp);
	if (old_size && ptr)
	{
		if (old_size < new_size)
			copy_size = old_size;
		else
			copy_size = new_size;
		ft_memmove(tmp, ptr, copy_size);
		free(ptr);
	}
	return (tmp);
}