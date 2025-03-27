/*
Assignment name  : rev_wstr
Expected files   : rev_wstr.c
Allowed functions: write, malloc, free
--------------------------------------------------------------------------------

Write a program that takes a string as a parameter, and prints its words in
reverse order.

A "word" is a part of the string bounded by spaces and/or tabs, or the
begin/end of the string.

If the number of parameters is different from 1, the program will display
'\n'.

In the parameters that are going to be tested, there won't be any "additional"
spaces (meaning that there won't be additionnal spaces at the beginning or at
the end of the string, and words will always be separated by exactly one space).

Examples:

$> ./rev_wstr "You hate people! But I love gatherings. Isn't it ironic?" | cat -e
ironic? it Isn't gatherings. love I But people! hate You$
$>./rev_wstr "abcdefghijklm"
abcdefghijklm
$> ./rev_wstr "Wingardium Leviosa" | cat -e
Leviosa Wingardium$
$> ./rev_wstr | cat -e
$
$>

*/
#include <unistd.h>

int main(int argc, char **argv)
{
	int start;
	int actual_word;
	int i = 0;
		
	if(argc == 2) //example: "Hola   mundo  "
	{   
		while(argv[1][i])
			i++;

		while(i >= 0) // we start here:  hola mundo  <-
		{
			while( argv[1][i] == '\0' || argv[1][i] == ' ' || argv[1][i] == '\t')
				i--;

			 actual_word = i;  //final of the word  hola mundo<-

			while(argv[1][i] && argv[1][i] != ' ' && argv[1][i] != '\t')
				i--; //go to word start possition : ->mundo
			
			start = i + 1;// the start of the word: m 
			int  flag = start;// aux

			while(start <= actual_word)
			{
				write (1, &argv[1][start],1); // output: "mundo"
				start++;		
			}

			if (flag)
				write(1, " ", 1); // output: "mundo "
		}
	}
	write(1, "\n", 1);
}