#include <stdlib.h>
#include <stdio.h>
#include "flood_fill.h"

// Helper function to create a modifiable copy of the zone
char** make_area(char** zone, t_point size)
{
    char** new;

    new = malloc(sizeof(char*) * size.y);
    for (int i = 0; i < size.y; ++i)
    {
        new[i] = malloc(size.x + 1);
        for (int j = 0; j < size.x; ++j)
            new[i][j] = zone[i][j];
        new[i][size.x] = '\0';
    }

    return new;
}

int main(void)
{
    // Define the grid size
    t_point size = {8, 5};
    
    // Define your grid
    char *zone[] = {
        "11111111",
        "10001001",
        "10010001",
        "10110001",
        "11100001",
    };

    // Create a modifiable copy
    char** area = make_area(zone, size);
    
    // Print original grid
    printf("Original grid:\n");
    for (int i = 0; i < size.y; ++i)
        printf("%s\n", area[i]);
    printf("\n");

    // Define starting point and call flood_fill
    t_point begin = {7, 4};
    flood_fill(area, size, begin);
    
    // Print modified grid
    printf("After flood fill:\n");
    for (int i = 0; i < size.y; ++i)
        printf("%s\n", area[i]);
    
    // Free memory
    for (int i = 0; i < size.y; ++i)
        free(area[i]);
    free(area);
    
    return (0);
}
