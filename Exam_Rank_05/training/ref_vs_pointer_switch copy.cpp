#include <iostream>

struct Node {
    int     value;
    Node*   next;
};

int main() {
    int values[] = {4, 7, -3, 9, -1};
    int size = 5;
    int *neg = findFirstNegative(values, size);
    int &third;
    int x = 1;
    int y = 2;

    if (neg)
        std::cout << "First negative: " << *neg << std::endl;
    else 
        std::cout << "no negative found" << std::endl;
    {
        int x = 1;
        int y = 2;
        swapByReference(x, y);
        std::cout << "after swapByReference: x=" << x << "y=";
    }
}