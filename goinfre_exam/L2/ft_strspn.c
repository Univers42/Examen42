/*
Assignment name	: ft_strspn
Expected files	: ft_strspn.c
Allowed functions: None
---------------------------------------------------------------

Reproduce exactly the behavior of the strspn function 
(man strspn).

The function should be prototyped as follows:

size_t	ft_strspn(const char *s, const char *accept);
*/
#include <stdio.h>

size_t ft_strspn(const char *s, const char *accept)
{
    size_t i = 0; 

    while (s[i]) 
    {
        size_t j = 0; 
        while (accept[j]) 
        {
            if (s[i] == accept[j]) //numero de coincidencias en j
                break; 
            j++;
        }
        if (accept[j] == '\0') 
            return i; 
        i++;
    }

    return i; 
}
/*

int main() {
    // Cadena de prueba y conjunto de caracteres
    const char *s1 = "hello, world";
    const char *accept1 = "he";

    const char *s2 = "abcdefg";
    const char *accept2 = "abc";

    const char *s3 = "12345";
    const char *accept3 = "123456789";

    // Probar ft_strspn con las cadenas
    size_t result;

    result = ft_strspn(s1, accept1);
    printf("Longitud del segmento inicial de '%s' que contiene solo caracteres en '%s': %zu\n", s1, accept1, result);

    result = ft_strspn(s2, accept2);
    printf("Longitud del segmento inicial de '%s' que contiene solo caracteres en '%s': %zu\n", s2, accept2, result);

    result = ft_strspn(s3, accept3);
    printf("Longitud del segmento inicial de '%s' que contiene solo caracteres en '%s': %zu\n", s3, accept3, result);

    return 0;
}
*/