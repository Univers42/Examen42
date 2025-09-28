/*Escribe un programa llamado alpha_mirror que tome una cadena y muestre esta cadena
después de reemplazar cada carácter alfabético por el carácter alfabético opuesto
carácter, seguido de una nueva línea.
 
'a' se convierte en 'z', 'Z' se convierte en 'A'
'd' se convierte en 'w', 'M' se convierte en 'N'
 
etcétera.
 
El caso no cambia.
 
Si el número de argumentos no es 1, muestra solo una nueva línea.
 
Examples:
 
$>./alpha_mirror "abc"
zyx
$>./alpha_mirror "My horse is Amazing." | cat -e
Nb slihv rh Znzarmt.$
$>./alpha_mirror | cat -e
$
$>*/
#include <unistd.h>

int	main(int argc, char **argv)
{
	int	i;

	i = 0;
	if (argc == 2)
	{
		while (argv[1][i])
		{
			if (argv[1][i] >= 'a' && argv[1][i] <= 'z')
				argv[1][i] = ('a' + 'z') - argv[1][i];
			else if (argv[1][i] >= 'A' && argv[1][i] <= 'Z')
				argv[1][i] = ('A' + 'Z') - argv[1][i];
			write(1, &argv[1][i], 1);
			i++;
		}
	}
	write(1, "\n", 1);
	return (0);
}
