#include <stdlib.h>
#include "get_next_line.h"

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	unsigned char	*old_ptr;
	unsigned char	*new_ptr;
	size_t			copy_size;

	if (new_size == 0)
		return (free(ptr), NULL);
	if (!ptr)
		return (malloc(new_size));
	old_ptr = (unsigned char *)ptr;
	new_ptr = (unsigned char *)malloc(new_size);
	if (!new_ptr)
		return (NULL);
	copy_size = old_size < new_size ? old_size : new_size;
	for (size_t i = 0; i < copy_size; ++i)
		new_ptr[i] = old_ptr[i];
	free(ptr);
	return (new_ptr);
}

void	dynstr_init(t_dynstr *str)
{
	str->len = 0;
	str->size = 0;
	str->data = NULL;
}

bool	dynstr_ensure_size(t_dynstr *str, size_t ensured)
{
	size_t	new_size;
	char	*new_data;

	if (ensured <= str->size)
		return (true);
	new_size = str->size;
	while (new_size < ensured)
		new_size = new_size ? new_size * 2 : 128;
	new_data = ft_realloc(str->data, str->size, new_size);
	if (!new_data)
		return (false);
	str->data = new_data;
	str->size = new_size;
	return (true);
}