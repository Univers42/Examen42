/*
Assignment name	: ft_strcspn
Expected files	: ft_strcspn.c
Allowed functions: None
---------------------------------------------------------------

Reproduce exactly the behavior of the function strcspn
(man strcspn).

The function should be prototyped as follows:

size_t	ft_strcspn(const char *s, const char *reject);
*/
#include <string.h>

size_t	ft_strcspn(const char *s, const char *reject)
{
    int i = 0; 
    int j = 0; 

    while(s[i])
    {
        j = 0;
        while(reject[j])
        {
            if(s[i] == reject[j]) //search the first apperance of a char in reject
                return i; 
            j++;
        }
        i++;
    }
    return (i); 
}