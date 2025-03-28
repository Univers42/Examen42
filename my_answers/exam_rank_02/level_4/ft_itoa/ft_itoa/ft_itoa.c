#include <stdlib.h>

static int	get_num_len(int n)
{
	int	len;

	len = (n <= 0);  // Start with 1 for negative sign or zero
	while (n)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int nbr)
{
	char	*str;
	int		len;
	int		sign;
	unsigned int	n;

	len = get_num_len(nbr);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	
	str[len] = '\0';
	
	// Handle sign with bitwise operation
	sign = nbr < 0;
	// Convert to positive using bitwise operations for absoluting
	n = sign ? (~nbr + 1) : nbr;
	
	// Handle case where nbr is 0
	if (n == 0)
		str[0] = '0';
		
	// Fill the string from right to left
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n /= 10;
	}
	
	// Add negative sign if necessary
	if (sign)
		str[0] = '-';
		
	return (str);
}
