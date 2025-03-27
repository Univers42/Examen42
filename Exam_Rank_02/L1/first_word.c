/*
Assignment name  : first_word
Expected files   : first_word.c
Allowed functions: write
--------------------------------------------------------------------------------

Write a program that takes a string and displays its first word, followed by a
newline.

A word is a section of string delimited by spaces/tabs or by the start/end of
the string.

If the number of parameters is not 1, or if there are no words, simply display
a newline.

Examples:

$> ./first_word "FOR PONY" | cat -e
FOR$
$> ./first_word "this        ...    is sparta, then again, maybe    not" | cat -e
this$
$> ./first_word "   " | cat -e
$
$> ./first_word "a" "b" | cat -e
$
$> ./first_word "  lorem,ipsum  " | cat -e
lorem,ipsum$
$> */
#include <unistd.h>

int main (int contador, char **cadena)
{
	int i = 0; //index
	
	if (contador == 2) //second arg 
	{
		while (cadena[1][i] == ' ' || cadena[1][i] == '	') //ignores spaces and tabs
			i++;
		while (!(cadena[1][i] == ' ' || cadena[1][i] == '	') && cadena[1][i]) //write the word and stops in tabs and spaces
			write(1, &cadena[1][i++], 1);
	}
	write(1, "\n", 1);
}
