/*
Assignment name	: ft_strpbrk
Expected files	: ft_strpbrk.c
Allowed functions: None
---------------------------------------------------------------

Reproduce exactly the behavior of the function strpbrk
(man strpbrk).

The function should be prototyped as follows:

char	*ft_strpbrk(const char *s1, const char *s2);*/
//#include <stdio.h>
#include <string.h>


char *ft_strpbrk(const char *s1, const char *s2)
{
	int i;
	
	while(*s1)
	{
		i = 0;
	   	while(s2[i])
		{
			if(*s1 == s2[i])
				return (char *) s1; // pointer to first apperance
			i++;
		}
		s1++;	
	}
	return (NULL);
}
/*
int main() {
    // Cadenas de prueba
    const char *s1 = "hello, world";
    const char *accept1 = "aeiou";

    const char *s2 = "abcdef";
    const char *accept2 = "xyz";

    const char *s3 = "123456789";
    const char *accept3 = "567";

    // Probar ft_strpbrk con las cadenas
    char *result;

    result = ft_strpbrk(s1, accept1);
    if (result != NULL) {
        printf("Primer carácter de '%s' que está en '%s': %c\n", s1, accept1, *result);
    } else {
        printf("Ningún carácter de '%s' se encontró en '%s'.\n", s1, accept1);
    }

    result = ft_strpbrk(s2, accept2);
    if (result != NULL) {
        printf("Primer carácter de '%s' que está en '%s': %c\n", s2, accept2, *result);
    } else {
        printf("Ningún carácter de '%s' se encontró en '%s'.\n", s2, accept2);
    }

    result = ft_strpbrk(s3, accept3);
    if (result != NULL) {
        printf("Primer carácter de '%s' que está en '%s': %c\n", s3, accept3, *result);
    } else {
        printf("Ningún carácter de '%s' se encontró en '%s'.\n", s3, accept3);
    }

    return 0;
}
*/