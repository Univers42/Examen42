/*
Assignment name  : ft_itoa
Expected files   : ft_itoa.c
Allowed functions: malloc
--------------------------------------------------------------------------------

Write a function that takes an int and converts it to a null-terminated string.
The function returns the result in a char array that you must allocate.

Your function must be declared as follows:

char	*ft_itoa(int nbr);
*/
#include <stdio.h>
#include <stdlib.h>

/*
ej:  0 y -42 ejemplos recomendados

1. saltas negativos l-37
2. longitud dividiendo entre 10 l-40
3. reservas cadena con esa longitud l-47 
4. añades el nulo para cerrar la cadena l-50
5. si hubiera sido 0 devuelves 0 l-52
6. es negativo añado - en posicion 0 l-58
7. copio la cadena hacia atras l-64
8. devuelvo result l-71
*/
char *ft_itoa(int nbr) 
{
    if(nbr == -2147483648)
        return("-2147483648\0"); //max int

    int n = nbr;  
    int len = 0;  
    char *result;

    if (nbr <= 0)
        len++;
    
    while (n) 
    {
        n /= 10;
        len++;
    }

    if (!(result = (char *)malloc(sizeof(char) * (len + 1)))) 
        return NULL; 
    
    result[len] = '\0'; 
    
    if (nbr == 0)
    {
        result[0] = '0';  
        return result;
    }
    
    if (nbr < 0) 
    {
        result[0] = '-';
        nbr = -nbr;
    }
    
    while (nbr) 
    {
        result[--len] = nbr % 10 + '0';  
        nbr /= 10;  
    }

    return result;  
}