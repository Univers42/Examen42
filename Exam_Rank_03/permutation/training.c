#include <stdio.h>
#include <stdlib.h>

/*
** Reverse-lexicographic step: find the next permutation in alphabetical order.
** 1) Find the rightmost index i where s[i] < s[i + 1] (the pivot).
** 2) Find the rightmost j > i with s[j] > s[i] (successor), swap i and j.
** 3) Reverse the suffix starting at i + 1 to restore minimal order.
** Returns 1 if a next permutation exists, 0 if we were at the last one.
*/
static int	next_permutation(char *s, int n)
{
	int	 i;
	int	 j;
	int	 l;
	int	 r;
	char tmp;

	i = n - 2;
	while (i >= 0 && s[i] >= s[i + 1])
		i--;
	if (i < 0)
		return (0);
	j = n - 1;
	while (s[j] <= s[i])
		j--;
	tmp = s[i];
	s[i] = s[j];
	s[j] = tmp;
	l = i + 1;
	r = n - 1;
	while (l < r)
	{
		tmp = s[l];
		s[l] = s[r];
		s[r] = tmp;
		l++;
		r--;
	}
	return (1);
}

/*
** In-place selection sort of the characters so we start from the smallest
** (alphabetical) arrangement, as required by the subject.
*/
static void	sort_chars(char *s, int n)
{
	int	 i;
	int	 j;
	int	 min;
	char tmp;

	i = 0;
	while (i < n - 1)
	{
		min = i;
		j = i + 1;
		while (j < n)
		{
			if (s[j] < s[min])
				min = j;
			j++;
		}
		if (min != i)
		{
			tmp = s[i];
			s[i] = s[min];
			s[min] = tmp;
		}
		i++;
	}
}

/*
** Simple strlen replacement: count characters until the null-terminator.
*/
static int	ft_strlen(const char *s)
{
	int i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	main(int argc, char **argv)
{
	char	*src;
	char	*pattern;
	int	 n;
	int	 i;

	/* Validate input */
	if (argc < 2 || !argv[1] || !argv[1][0])
		return (0);
	src = argv[1];
	
	/* Duplicate argv[1] to a writable buffer (do not modify argv directly). */
	n = ft_strlen(src);
	pattern = (char *)malloc((size_t)n + 1);
	if (!pattern)
		return (1);
	i = 0;
	while (i < n)
	{
		pattern[i] = src[i];
		i++;
	}
	pattern[n] = '\0';

	/* Start from the sorted string, then iterate next_permutation. */
	sort_chars(pattern, n);
	puts(pattern);
	while (next_permutation(pattern, n))
		puts(pattern);

	free(pattern);
	return (0);
}