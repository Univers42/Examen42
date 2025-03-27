/*
Assignment name  : do_op
Expected files   : *.c, *.h
Allowed functions: atoi, printf, write
--------------------------------------------------------------------------------

Escribe un programa que tenga tres cadenas:
- El primero y el tercero son representaciones de números enteros con signo de base 10.
  que caben en un int.
- El segundo es un operador aritmético elegido entre: + - * / %

El programa debe mostrar el resultado de la operación aritmética solicitada,
seguido de una nueva línea. Si el número de parámetros no es 3, el programa
solo muestra una nueva línea.

Puede asumir que la cadena no tiene errores ni caracteres extraños. Negativo
Los números, en entrada o salida, tendrán uno y solo un '-' inicial. El
El resultado de la operación cabe en un int.

Examples:

$> ./do_op "123" "*" 456 | cat -e
56088$
$> ./do_op "9828" "/" 234 | cat -e
42$
$> ./do_op "1" "+" "-43" | cat -e
-42$
$> ./do_op | cat -e
*/
#include <stdlib.h>
#include <stdio.h>
//+ - * / %
int main(int argc, char **argv)
{
	if (argc == 4)
	{
		if (argv[2][0] == '+')
			printf("%d", atoi(argv[1]) + atoi(argv[3]));
		else if (argv[2][0] == '-')
			printf("%d", atoi(argv[1]) + atoi(argv[3]));
		else if (argv[2][0] == '*')
			printf("%d", atoi(argv[1]) * atoi(argv[3]));
		else if (argv[2][0] == '/')
			printf("%d", atoi(argv[1]) / atoi(argv[3]));
		else if (argv[2][0] == '%')
			printf("%d", atoi(argv[1]) % atoi(argv[3]));
	}
	printf("\n"); 
	return 0; 
}