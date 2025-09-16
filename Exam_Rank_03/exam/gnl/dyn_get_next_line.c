#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

int strclen(char *str, int c, int *d)
{
	char *cpy;
	bool reached;

	reached = false;
	cpy = str;
	while (*cpy)
	{
		if (*cpy == (char)c)
		{
			reached = true;
			cpy++;
			break;
		}
		cpy++;
	}
	*d = (cpy - str);
	return (reached);
}

void    *ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	if (s == d)
		return (dest);
	if (s < d)
	{
		while(n--)
			*d++ = *s++;
	}
	else
	{
		s += n;
		d += n;
		while (n--)
			*--d = *--s;
	}
	return (dest);
}

char *ft_strdup(char *str)
{
	char	*dup;
	int		len;

	if (!str)
		return (NULL);
	strclen(str, '\0', &len);
	dup = malloc(sizeof(char) * len + 1);
	if(!dup)
		return (NULL);
	ft_memmove(dup, str, len);
	dup[len] = '\0';
	return (dup);
}

char *ft_strjoin(const char *src1, const char *src2)
{
	char *joined;
	int	len1;
	int	len2;

	if (!src1 || !src2)
		return (NULL);
	strclen(src1, '\0', &len1);
	strclen(src2, '\0', &len2);
	joined = malloc(sizeof(char) * (len1 + len2 + 1));
	if (!joined)
		return (NULL);
	ft_memmove(joined, src1, len1);
	ft_memmove(joined + len1, src2, len2);
	joined[len1 + len2] = '\0';
	return (joined);
}

__attribute__((weak))
int main(int argc, char **argv)
{
    bool reached;
    char memdup[30];
    int d;

    // --- TEST 1: strclen ---
    printf("=== TEST strclen ===\n");
    ++argv; // skip program name
    while (*argv)
    {
        reached = strclen(*argv, 'e', &d);
        ft_memmove(memdup, *argv, d);
        memdup[d] = '\0';
        printf("Input       : %s\n", *argv);
        printf("Result      : %s\n", reached ? "found 'e'" : "no 'e'");
        printf("Length      : %d\n", d);
        printf("Prefix copy : %s\n\n", memdup);
        argv++;
    }

    // --- TEST 2: ft_strdup ---
    printf("=== TEST ft_strdup ===\n");
    char *dup = ft_strdup("banana");
    printf("Original : %s\n", "banana");
    printf("Duplicate: %s\n\n", dup);
    free(dup);

    // --- TEST 3: ft_strjoin ---
    printf("=== TEST ft_strjoin ===\n");
    char *joined = ft_strjoin("Hello, ", "world!");
    printf("Joined : %s\n\n", joined);
    free(joined);

    // --- TEST 4: ft_memmove overlap case ---
    printf("=== TEST ft_memmove (overlap) ===\n");
    char buffer[20] = "overlapping";
    printf("Before : %s\n", buffer);
    ft_memmove(buffer + 2, buffer, 5); // shift "overl" into itself
    buffer[7] = '\0';
    printf("After  : %s\n", buffer);

    return 0;
}