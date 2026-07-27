# include "box.hpp"
# include <iostream>

Box::Box(int val) {
    std::cout << "Constructor called \n";
    value = new int(val);
}

Box::Box(const Box& other) {
    std::cout << "Copy constructor called\n ";
    value = new int(*other.value);
}

Box& Box::operator=(const Box& other) {
    std::cout << "Copy assignment operator called\n";
    if (this != &other) {
        delete value;
        value = new int(*other.value);
    }
    return (*this);
}

Box::~Box() {
    std::cout << "Destructor called \n";
    delete value;
}

void Box::show() const {
    std::cout << "Box value:" << *value << std::endl; 
}

