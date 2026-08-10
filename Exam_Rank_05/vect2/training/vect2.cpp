#include "vect2.hpp"

vect2::vect2(void) : _x(0), _y(0) {}

vect2::vect2(int x, int y) : _x(x), _y(y) {}

vect2::vect2(const vect2& other) : _x(other._x), _y(other._y) {}

vect2& vect2::operator=(const vect2& other) {
    if (this != &other) {
        _x = other._x;
        _y = other._y;
    }
    return (*this);
}

vect2::~vect2(void) {}

int& vect2::operator[](const int idx) { return (idx == 0 ? this->_x : this->_y); }

int vect2::operator[](const int idx) const { return (idx == 0 ? this->_x : this->_y); }

vect2& vect2::operator++(void) { return (*this += 1); }

vect2 vect2::operator++(int) {
    vect2 tmp(*this);
    ++(*this);
    return tmp;
}

vect2 vect2::operator--(int) {
    vect2 tmp(*this);
    --(*this);
    return tmp;
}

vect2& vect2::operator--(void) { return (*this -= 1); }

std::ostream& operator<<(std::ostream& os, const vect2& other) {
    return (os << "{" << other[0] << ", " << other[1] << "}");
}