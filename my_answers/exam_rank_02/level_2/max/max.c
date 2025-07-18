#include <stdio.h>

/**
 * Why this implementation might be faster than the usual one ?
 * The traditional if(tab[i] > max_val) approach causes a branch in the code,
 * our bitwise approach replace the conditional branch with pure computation
 * `-(tab[i] > max_val)` is either 0 or -1, it's a mask
 * `(mask & tab[i]) keeps tab[i] if it's greater than max_val
 * the OR combines these to select the larger value
 * 
 * 5  = 00000000 00000000 00000000 00000101
 * 12 = 00000000 00000000 00000000 00001100
 * 3  = 00000000 00000000 00000000 00000011
 * 8  = 00000000 00000000 00000000 00001000
 * mask = 
 * 
 * 1ST iteration
 * compare: 
 * tab[1] > max_value [all 1s or 0s if false]
 * 12 > 5 = TRUE so all 1S
 * mask = -(1) = 11111111 11111111 11111111 11111111 (all 1s) ==> negative 1 in twos complement gives all bits set to 1
 * apply mask comparation:
 * mask & tab[1] = 11111111 11111111 11111111 11111111 
 *                 & 00000000 00000000 00000000 00001100
 *                 --------------------------------
 *                 00000000 00000000 00000000 00001100 (12)
 * 
 * ~mask = 00000000 00000000 00000000 00000000 00000000 (all 0s)
 * ~mask & max_val = 00000000 00000000 00000000 00000000
    *              & 00000000 00000000 00000000 00000101
 *                  --------------------------------
 *                  00000000 00000000 00000000 00000000 (0)
 * (mask & tab[1] | (~mask & max_val)) = 00000000 00000000 00000000 00001100
 *                                       0000000000 00000000 00000000
                        *                  --------------------------------    
                        *                  00000000 00000000 00000000 00001100 (12)
 */ 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int max(int *tab, unsigned int len)
{
    if (len == 0) 
        return 0;
        
    int max_val = tab[0];
    for (unsigned int i = 1; i < len; i++)
    {
        int mask = -(tab[i] > max_val);
        max_val = (mask & tab[i]) | (~mask & max_val);
    }
    return max_val;
}

int main(int argc, char **argv)
{
    unsigned int len = 100;
    int *tab = (int *)malloc(sizeof(int) * len);
    if (!tab)
        return 1;
    srand(time(NULL));
    for (unsigned int i = 0; i < len; i++)
        tab[i] = rand() % 1000;
    int result = max(tab, len);    
    printf("The maximum value is : %d \n", result);
    free(tab);
    return 0;
}
