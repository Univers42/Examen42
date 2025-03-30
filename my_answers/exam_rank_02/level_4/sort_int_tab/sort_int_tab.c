#include <stdio.h>
#include <stdlib.h>
// Safe XOR swap that checks for pointer equality
void xor_swap(int *a, int *b)
{
    if (a != b) // Prevent XOR swap with self (which would zero out the value)
    {
        *a ^= *b;
        *b ^= *a;
        *a ^= *b;
    }
}

// Insertion sort optimized for small arrays
void insertion_sort(int *tab, unsigned int size)
{
    for (unsigned int i = 1; i < size; i++)
    {
        int temp = tab[i];
        int j = i;
        
        while (j > 0 && tab[j - 1] > temp)
        {
            tab[j] = tab[j - 1];
            j--;
        }
        tab[j] = temp;
    }
}

// Selects median-of-three as pivot and places it at high position
int select_pivot(int *tab, int low, int high)
{
    // Get middle index using bitshift (divide by 2)
    int mid = low + ((high - low) >> 1);
    
    // Sort low, mid, high elements using XOR swap
    if (tab[mid] < tab[low])
        xor_swap(&tab[mid], &tab[low]);
    if (tab[high] < tab[low])
        xor_swap(&tab[high], &tab[low]);
    if (tab[mid] < tab[high])
        xor_swap(&tab[mid], &tab[high]);
    
    // Return the pivot value (now at high position)
    return tab[high];
}

// Partition the array around pivot and return pivot position
int partition(int *tab, int low, int high, int pivot)
{
    int i = low - 1;
    
    for (int j = low; j < high; j++)
    {
        if (tab[j] <= pivot)
        {
            i++;
            xor_swap(&tab[i], &tab[j]);
        }
    }
    
    int pivot_pos = i + 1;
    xor_swap(&tab[pivot_pos], &tab[high]);
    
    return pivot_pos;
}

// Quick sort implementation with tail call optimization
void quick_sort(int *tab, int low, int high)
{
    while (low < high)
    {
        // Select pivot using median-of-three
        int pivot = select_pivot(tab, low, high);
        
        // Partition and get pivot position
        int pivot_pos = partition(tab, low, high, pivot);
        
        // Calculate partition sizes using direct subtraction
        unsigned int left_size = pivot_pos - low;
        unsigned int right_size = high - pivot_pos;
        
        // Recursively sort smaller partition, iteratively sort larger partition
        if (left_size < right_size)
        {
            // Recursively sort smaller left partition
            if (left_size > 0)
                quick_sort(tab + low, 0, left_size - 1);
            
            // Iteratively handle right partition
            low = pivot_pos + 1;
        }
        else
        {
            // Recursively sort smaller right partition
            if (right_size > 0)
                quick_sort(tab + pivot_pos + 1, 0, right_size - 1);
            
            // Iteratively handle left partition
            high = pivot_pos - 1;
        }
    }
}

// Main sorting function
void sort_int_tab(int *tab, unsigned int size)
{
    // Base case
    if (size <= 1)
        return;
    
    // Use insertion sort for small arrays
    if (size <= 16)
    {
        insertion_sort(tab, size);
        return;
    }
    
    // Use quick sort for larger arrays
    quick_sort(tab, 0, size - 1);
}

// Main function to test sort_int_tab
int main(void)
{
    int arr[] = {5, 3, 8, 1, 2, 7, 4, 6};
    unsigned int size = sizeof(arr) / sizeof(arr[0]);

    printf("Before sorting: ");
    for (unsigned int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");

    sort_int_tab(arr, size);

    printf("After sorting:  ");
    for (unsigned int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}
