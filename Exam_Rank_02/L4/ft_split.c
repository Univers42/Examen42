/*
Assignment name  : ft_split
Expected files   : ft_split.c
Allowed functions: malloc
--------------------------------------------------------------------------------

Write a function that takes a string, splits it into words, and returns them as
a NULL-terminated array of strings.

A "word" is defined as a part of a string delimited either by spaces/tabs/new
lines, or by the start/end of the string.

Your function must be declared as follows:

char    **ft_split(char *str);
*/

#include <stdlib.h>

char		**ft_split(char *str)
{
    int     i = 0;
    int     j;
    int     k = 0; 
	char	**split;

	if (!(split = (char **)malloc(sizeof(char *) * 256)))
		return (NULL);
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        i++; 

	while (str[i])
	{
		j = 0;
		if (!(split[k] = (char *)malloc(sizeof(char) * 4096)))
			return (NULL);

		while (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i])
			split[k][j++] = str[i++];

        while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
            i++;
		split[k][j] = '\0';
		k ++;
	}
	split[k] = NULL;
	return (split);
}
/*
int main() {
    char str[] = "   Esto es  un    ejemplo   de   split. ";
    char **result;
    int i;

    result = ft_split(str);

    if (!result) {
        fprintf(stderr, "Error al dividir la cadena.\n");
        return (1);
    }

    i = 0;
    while (result[i]) {
        printf("Palabra %d: '%s'\n", i, result[i]);
        free(result[i]); // Libera cada palabra
        i++;
    }

    free(result); // Libera el array de punteros

    return (0);
}
*/
