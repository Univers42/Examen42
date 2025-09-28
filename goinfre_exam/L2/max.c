/*
Assignment name  : max
Expected files   : max.c
Allowed functions:
--------------------------------------------------------------------------------

Escribe la siguiente función:

int max(int* tab, unsigned int len);

El primer parámetro es una matriz de int, el segundo es el número de elementos en
la matriz.

La función devuelve el número más grande encontrado en la matriz.

Si la matriz está vacía, la función devuelve 0.
*/
#include<stdio.h>

int		max(int* tab, unsigned int len)
{
	unsigned int	i = 0;
  int result; 

	if(len == 0)
		return(0);
  
	while (i < len)
    {
        if (result < tab[i])
            result = tab[i]; 
        i++;
    }
    return result; 
}