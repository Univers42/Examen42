#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// dynamic programming is breka the problem into sub problems, solve each problem once, and reuse the result
// naive solution.
size_t fib(size_t n) {
	if (n == 0 || n == 1)
		return (n);
	return (fib(n - 2) + fib(n - 1));
}

//memoization technique = top down approach
// index 0 1 2 3 4 5 6 
// --------------------
// value ? ? ? ? ? ? ?

size_t mem_fib(size_t n, size_t *memo) {
	if (n <= 1)
		return (n);
	if (memo[n] != -1)
		return (memo[n]);
	memo[n] = mem_fib(n - 1, memo) + mem_fib(n - 2, memo);
	return (memo[n]);
}


// bottom up approach in recursive
void weird_fibonaci(size_t i, size_t n, int *dp){
	if (i >= n)
		return;
	dp[i] = dp[i - 1] + dp[i - 2];
	weird_fibonaci(i + 1, n, dp);
}


size_t fibonacci_efficient_memory(size_t n){
	
	size_t dp[2] = {0, 1};
	size_t next;

	if (n == 0)
		return (0);
	else if (n == 1)
		return (1);
     	for (size_t i = 0; i < n; i++)
	{
		next = dp[0]  + dp[1];
		dp[0] = dp[1];
		dp[1] = next;
	}
	return (dp[1]);
}

size_t fibonnaci(size_t *map, size_t n, size_t *known) {
	for (size_t i = *known + 1; i < n; i++) {
		*fibonacci
	}
}

// bottom cup approach in iteration
//


int main(void){
	size_t memo[1000];
	memset(memo, -1, sizeof(memo));
	for (int i  = 0; i < 100; ++i)
		printf("%lld\n", mem_fib(i, memo));
}
