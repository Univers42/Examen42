#include "flood_fill.h"

// Helper function to perform DFS recursively
void fill(char **tab, t_point size, int x, int y, char target)
{
    // Check bounds and if current cell has target character
    if (x < 0 || x >= size.x || y < 0 || y >= size.y || tab[y][x] != target)
        return;
    
    // Fill current cell with 'F'
    tab[y][x] = 'F';
    
    // Recursively fill in all four directions (non-diagonal)
    fill(tab, size, x + 1, y, target); // right
    fill(tab, size, x - 1, y, target); // left
    fill(tab, size, x, y + 1, target); // down
    fill(tab, size, x, y - 1, target); // up
}

void flood_fill(char **tab, t_point size, t_point begin)
{
    // Get the target character from the beginning point
    if (begin.x < 0 || begin.x >= size.x || begin.y < 0 || begin.y >= size.y)
        return;
        
    char target = tab[begin.y][begin.x];
    
    // Start the fill process from the beginning point
    fill(tab, size, begin.x, begin.y, target);
}
