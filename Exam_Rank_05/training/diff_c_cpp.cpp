#include <iostream>

int main(){
    int b = 5;
    int *p = &b;
    int &r = b;

    std::cout << "value of b initialized : \t" << &b << b << std::endl;
    std::cout << "value of pointer p that reference b (expected 5) : \t" << p << *p << std::endl;
    std::cout << "value of references taht initialize to r : \t" << &r << r << std::endl;
    return (0);
}
