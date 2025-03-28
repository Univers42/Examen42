#include <unistd.h>
void ft_swap(int *a, int *b)
{
    (*a ^= *b), (*b ^= *a), (*a ^= *b);
}


// Function to print an integer
void ft_putnbr(int nb)
{
    char c;
    
    if (nb < 0)
    {
        write(1, "-", 1);
        nb = -nb;
    }
    if (nb >= 10)
        ft_putnbr(nb / 10);
    c = nb % 10 + '0';
    write(1, &c, 1);
}

// Function to print a new line
void ft_putchar(char c)
{
    write(1, &c, 1);
}

int main(void)
{
    // Basic usage - Swapping two variables
    int x = 42;
    int y = 21;
    
    // Print initial values
    write(1, "Before swap: x = ", 17);
    ft_putnbr(x);
    write(1, ", y = ", 6);
    ft_putnbr(y);
    ft_putchar('\n');
    
    // Call ft_swap to swap the values
    ft_swap(&x, &y);
    
    // Print values after swapping
    write(1, "After swap:  x = ", 17);
    ft_putnbr(x);
    write(1, ", y = ", 6);
    ft_putnbr(y);
    ft_putchar('\n');
    
    // Example 2: Using ft_swap in bubble sort
    int arr[5] = {5, 3, 8, 1, 2};
    int i, j;
    
    write(1, "\nArray before sorting: ", 22);
    for (i = 0; i < 5; i++)
    {
        ft_putnbr(arr[i]);
        write(1, " ", 1);
    }
    ft_putchar('\n');
    
    // Bubble sort using ft_swap
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4 - i; j++)
        {
            if (arr[j] > arr[j + 1])
                ft_swap(&arr[j], &arr[j + 1]);
        }
    }
    
    write(1, "Array after sorting:  ", 22);
    for (i = 0; i < 5; i++)
    {
        ft_putnbr(arr[i]);
        write(1, " ", 1);
    }
    ft_putchar('\n');
    
    return (0);
}
