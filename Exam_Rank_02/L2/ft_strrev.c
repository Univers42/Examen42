
/*
Assignment name  : ft_strrev
Expected files   : ft_strrev.c
Allowed functions: 
--------------------------------------------------------------------------------

Write a function that reverses (in-place) a string.

It must return its parameter.

Your function must be declared as follows:

char    *ft_strrev(char *str);
*/
//#include <stdio.h>
#include <unistd.h>

int		ft_strlen(char *str)
{
	int i = 0; 

	while(str[i])
		i++;
	return i; 
}

char    *ft_strrev(char *str)
{
    int inicio = 0;
    int fin = ft_strlen(str) - 1;
    char temp;

    while (inicio < fin)
    {
        temp = str[inicio];
        str[inicio] = str[fin]; //swap para cambiar las posiciones inicial y final
        str[fin] = temp;
        inicio++;
        fin--;
    }

    return str;
}
/*
int main()
{
    // Test strings
    char str1[] = "hello";
    // Show original strings
    printf("Original: %s\n", str1);
    
    // Reverse and show the first string
    printf("Reversed: %s\n", ft_strrev(str1));

    return 0;
}
*/