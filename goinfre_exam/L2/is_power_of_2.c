/*
Assignment name  : is_power_of_2
Expected files   : is_power_of_2.c
Allowed functions: None
--------------------------------------------------------------------------------

Write a function that determines if a given number is a power of 2.

This function returns 1 if the given number is a power of 2, otherwise it returns 0.

Your function must be declared as follows:

int	    is_power_of_2(unsigned int n);
*/
int is_power_of_2(unsigned int n)
{
    if (n == 0)
        return 0;
    return ((n & (n - 1)) == 0); //binary example 4: 0100 & 0011 = 0100 
}