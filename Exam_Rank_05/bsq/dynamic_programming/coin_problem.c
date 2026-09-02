#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Given a set of coin value coins={c1,c2,...,CK} and a target sum of money m, what's the minimum number of coing that form the sum n ?


size_t min_coins(size_t *coins, size_t target)
{
	if (target)
}


/**
 * Dynamic programming 
 * minimum_coings(coins, m) return the minimum number of coins required for a sum m
 * minimum_coings(coings, 0) = 0
 * min_coins(coins, m) = ?
 * 
 *
 * */

size_t min_coins(size_t *coins, size_t target)
{
	if (target == 0)
		return (0);

}


int main( void ){
	size_t coins[3] = {1, 3, 4};
	size_t target = 6;

	// 6 = 3 + 3
	// 6 = 1 + 1 + 4
	// 6 = 1 + 1 + 1+ 1 +1 +1
	printf("%zd\n", min_coings(coins, target));
	return (0);
}
