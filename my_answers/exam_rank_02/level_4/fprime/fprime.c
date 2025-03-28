#include <stdio.h>
#include <stdlib.h>

void fprime(int nb)
{
	int first_factor = 1;
	if (nb == 1)
	{
		printf("1");
		return ;
	}
	while (( nb & 1) == 0)		// equivalent to nb % 2 == 0, but faster
	{
		if (!first_factor)
			printf("*");
		printf("2");
		first_factor = 0;
		nb >>= 1;		//equivalent to nb /= 2, but faster
	}

	for (int factor = 3; factor * factor <= nb; factor += 2)
	{
		while(nb % factor == 0)
		{
			if (!first_factor)
				printf("*");
			printf("%d", factor);
			first_factor = 0;
			nb /= factor;
		}
	}

	if (nb > 1)
	{
		if (!first_factor)
			printf("*");
		printf("%d", nb);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("\n");
		return (0);
	}

	int nb = atoi(argv[1]);
	fprime(nb);
	printf("\n");
	return (0);
}
