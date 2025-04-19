#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

# define NEWLINE() (write(1,"\n",1))
#define GET_BIT(arr, i)   (arr[(i) >> 3] &  (1 << ((i) & 7)))
#define CLEAR_BIT(arr, i) (arr[(i) >> 3] &= ~(1 << ((i) & 7)))
#define SET_BIT(arr, i)   (arr[(i) >> 3] |=  (1 << ((i) & 7)))

// Trial division [NOT EFFICIENT but still useful]
int is_prime(int number)
{
    int i;

    i = 2;
    if(number < 2)
        return (0);
    while(i < sqrt(number))
    {
        if(number % i == 0)
            return 0;
        i++;
    }
    return (1);
}

void trial_division_prime_numbers(int number)
{
    int i;
    for (int i = 2; i < number; i++)
    {
        if (is_prime(i))
            printf("%d\t", i);
    }
    NEWLINE();
}

/**
 * Erathostene way
 */
void mark_multiples(int *is_prime, int start, int number)
{
    for (int j = start * start; j <= number; j += start)
        is_prime[j] = 0;
}

void erathostene(int number)
{
    int *is_prime = malloc((number + 1) * sizeof(int));
    if (!is_prime)
        return;

    // 1. Assume all numbers are prime
    for (int i = 0; i <= number; i++)
        is_prime[i] = 1;

    // 2. 0 and 1 are not prime
    is_prime[0] = is_prime[1] = 0;

    // 3. Mark multiples of each prime
    for (int i = 2; i * i <= number; i++)
    {
        if (is_prime[i])
            mark_multiples(is_prime, i, number);
    }

    // 4. Print primes
    for (int i = 2; i <= number; i++)
    {
        if (is_prime[i])
            printf("%d\t", i);
    }
    NEWLINE();
    free(is_prime);
}


void bitwise_sieve(int n)
{
    int size = (n >> 4) + 1; 
    uint8_t *is_prime = malloc(size);
    int count = 1;

    if (!is_prime)
        return;
    for (int i = 0; i < size; i++) 
        is_prime[i] = 0xFF;
    printf("2\t");
    int sqrt_n = sqrt(n);
    for (int i = 3; i <= n; i += 2)
    {
        int idx = i >> 1;
        if (GET_BIT(is_prime, idx))
        {
            printf("%d\t", i);
            if (i <= sqrt_n) 
            {
                for (int j = i * i; j <= n; j += 2 * i)
                {
                    int jidx = j >> 1;
                    CLEAR_BIT(is_prime, jidx);
                }
            }
        count++;
        }
    }
    printf("\n%d\n", count);
    free(is_prime);
}


int main(int argc, char **argv)
{
    int number;
    if(argc != 2)
        return (NEWLINE(), 1);
    number = atoi(argv[1]);
    bitwise_sieve(number);
   return (0);
}


// Miller-Rabin primality test for n, k iterations
//int miller_rabin(int n, int k)
//{
//    if (n < 2) return 0;
//    if (n == 2 || n == 3) return 1;
//    if (n % 2 == 0) return 0;
//
//    // Write n-1 as 2^r * d
//    int r = 0;
//    int d = n - 1;
//    while (d % 2 == 0) {
//        d /= 2;
//        r++;
//    }
//
//    for (int i = 0; i < k; i++) {
//        int a = 2 + rand() % (n - 3); // random base in [2, n-2]
//        int x = mod_pow(a, d, n);      // compute a^d % n
//        if (x == 1 || x == n - 1)
//            continue;
//        int continue_outer = 0;
//        for (int j = 0; j < r - 1; j++) {
//            x = mod_pow(x, 2, n);
//            if (x == n - 1) {
//                continue_outer = 1;
//                break;
//            }
//        }
//        if (continue_outer)
//            continue;
//        return 0; // composite
//    }
//    return 1; // probably prime
//}
//
//
//int sieve_of_atkin(int limit)
//{
//    char *is_prime = calloc(limit + 1, sizeof(char));
//    int sqrt_limit = sqrt(limit);
//    int n;
//    for(int x = 1; x <=  sqrt_limit; x++)
//    {
//        for (int y = 1; y <= sqrt_limit; y++)
//        {
//            n = 4*x*x + y*y;
//            if(n <= limit && (n % 12 == 1 || n % 12 == 5))
//                is_prime[n] ^= 1;
//            n = 3*x*x + y+y;
//            if(x > y && n <= limit && n % 12 == 11)
//                is_prime[n] ^= 1;  
//        }
//    }
//    for (int n = 5; n <= sqrt_limit; n++)
//    {
//        if (is_prime[n])
//        {
//            int k = n * n;
//            for (int i = k; i <= limit; i += k)
//                is_prime[i] = 0;
//        }
//    }
//    printf("2 3");
//    for (int n = 5; n <= limit; n++)
//        if(is_prime[n])
//            printf("%d", n);
//        printf("\n");
//    free(is_prime);
//}
