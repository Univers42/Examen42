#include <stdlib.h>

int *ft_range(int start, int end)
{
    int *range;
    int i;
    int size;
    int step = 1;
    
    // Calculate size and step based on direction (ascending or descending)
    if (start <= end)
    {
        size = end - start + 1;
    }
    else
    {
        size = start - end + 1;
        step = -1;
    }
    
    // Allocate memory for the range
    range = (int *)malloc(sizeof(int) * size);
    if (!range)
        return NULL;
    
    // Fill the array with consecutive values
    for (i = 0; i < size; i++)
    {
        range[i] = start + (i * step);
    }
    
    return range;
}

// Test function
#include <stdio.h>

void print_range(int *arr, int size)
{
    int i;
    
    printf("[");
    for (i = 0; i < size; i++)
    {
        printf("%d", arr[i]);
        if (i < size - 1)
            printf(", ");
    }
    printf("]\n");
}

int main(void)
{
    int *range;
    int size;
    
    // Test case 1: Ascending range
    range = ft_range(1, 3);
    size = 3;
    printf("ft_range(1, 3): ");
    print_range(range, size);
    free(range);
    
    // Test case 2: Range including negative numbers
    range = ft_range(-1, 2);
    size = 4;
    printf("ft_range(-1, 2): ");
    print_range(range, size);
    free(range);
    
    // Test case 3: Single element range
    range = ft_range(0, 0);
    size = 1;
    printf("ft_range(0, 0): ");
    print_range(range, size);
    free(range);
    
    // Test case 4: Descending range
    range = ft_range(0, -3);
    size = 4;
    printf("ft_range(0, -3): ");
    print_range(range, size);
    free(range);
    
    // Additional test case: Large range
    range = ft_range(-10, 10);
    size = 21;
    printf("ft_range(-10, 10): ");
    print_range(range, size);
    free(range);
    
    return 0;
}