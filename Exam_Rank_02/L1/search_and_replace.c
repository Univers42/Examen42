/*Assignment name  : search_and_replace
Expected files   : search_and_replace.c
Allowed functions: write, exit
--------------------------------------------------------------------------------

Escribe un programa llamado search_and_replace que tome 3 argumentos, el primero
argumentos es una cadena en la que se reemplaza una letra (segundo argumento) por
otro (tercer argumento).

Si el número de argumentos no es 3, simplemente muestre una nueva línea.

Si el segundo argumento no está contenido en el primero (la cadena)
luego el programa simplemente reescribe la cadena seguida de una nueva línea.

Examples:
$>./search_and_replace "Papache est un sabre" "a" "o"
Popoche est un sobre
$>./search_and_replace "zaz" "art" "zul" | cat -e
$
$>./search_and_replace "zaz" "r" "u" | cat -e
zaz$
$>./search_and_replace "jacob" "a" "b" "c" "e" | cat -e
$
$>./search_and_replace "ZoZ eT Dovid oiME le METol." "o" "a" | cat -e
ZaZ eT David aiME le METal.$
$>./search_and_replace "wNcOre Un ExEmPle Pas Facilw a Ecrirw " "w" "e" | cat -e
eNcOre Un ExEmPle Pas Facile a Ecrire $*/
#include <unistd.h>

int main(int argc, char **argv)
{
    int i = 0;
    
    if (argc == 4 && !argv[2][1] && !argv[3][1])
    {
        while(argv[1][i])
        {
            if (argv[1][i] == argv[2][0])
                argv[1][i] = argv[3][0]; 
            write(1, &argv[1][i], 1);
            i++; 
        }
    }
    write(1, "\n", 1);
}

