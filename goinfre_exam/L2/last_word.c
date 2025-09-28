#include <unistd.h>

int		main(int argc, char **argv)
{
	if (argc == 2)
	{
		int i = 0;
		int end = 0; 
		int start = 0;

		while (argv[1][i])
			i++;

		while( argv[1][i] == '\0' || argv[1][i] == ' ' || argv[1][i] == '\t')
			i--;
			 end = i;  //final of the word  hola mundo<-

		while(argv[1][i] && argv[1][i] != ' ' && argv[1][i] != '\t')
			i--; //go to word start possition : ->mundo
			
		start = i + 1;// the start of the word: m 

		// Print the last word
		while (start <= end)
			write(1, &argv[1][start++], 1);
	}
	write(1, "\n", 1);
	return (0);
}

/*
Assignment name  : last_word
Expected files   : last_word.c
Allowed functions: write
--------------------------------------------------------------------------------

Write a program that takes a string and displays its last word followed by a \n.

A word is a section of string delimited by spaces/tabs or by the start/end of
the string.

If the number of parameters is not 1, or there are no words, display a newline.

Example:

$> ./last_word "FOR PONY" | cat -e
PONY$
$> ./last_word "this        ...       is sparta, then again, maybe    not" | cat -e
not$
$> ./last_word "   " | cat -e
$
$> ./last_word "a" "b" | cat -e
$
$> ./last_word "  lorem,ipsum  " | cat -e
lorem,ipsum$
$>
*/