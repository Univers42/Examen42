
/*
Assignment name  : paramsum
Expected files   : paramsum.c
Allowed functions: write
--------------------------------------------------------------------------------

Write a program that displays the number of arguments passed to it, followed by
a newline.

If there are no arguments, just display a 0 followed by a newline.

Example:

$>./paramsum 1 2 3 5 7 24
6
$>./paramsum 6 12 24 | cat -e
3$
$>./paramsum | cat -e
0$
$>
*/

int     ft_putnbr(int n)
{
    char number[] = "0123456789";

    while(number[n] > 9)
        ft_putnbr(n / 10); 
    write(1, &number[n % 10], 1); 
}
int main(int argc, char **argv)
{

    if(argc > 2)
    {
        ft_putnbr(argc - 1); 
    }
    if (argc <= 2)
        ft_putnbr(0); 
    write(1, "\n" , 1); 
    
    return 0; 
}