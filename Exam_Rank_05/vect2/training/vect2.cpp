#include "vect2.hpp"

vect2::vect2(): x(0), y(0) {/*default ctor*/}
vect2::vect2(const int x, const int y): x(x) , y(y){/*ctor secundary*/}
vect2::vect2(const vect2& other): x(other.x), y(other.y){/*copy ctor*/}
vect2::~vect2() {/*destructor*/}
int     vect2::operator[](const int idx) const {return (idx == 0 ? this->x : this->y);}
int&    vect2::operator[](const int idx) {return (idx == 0 ? this->x : this->y);}
vect2&  vect2::operator=(const vect2& other) {this->x = other.x; this->y = other.y; return *this;}
vect2&  vect2::operator+=(const vect2& other) {this->x += other.x; this->y += other.y; return *this;}
vect2&  vect2::operator-=(const vect2& other) {this->x -= other.x; this->y -= other.y; return *this;}
vect2&  vect2::operator*=(const int n) {this->x *= n; this->y *= n; return *this;}
vect2   vect2::operator+(const vect2& other)const {return (vect2(this->x + other.x, this->y + other.y));}
vect2   vect2::operator-(const vect2& other)const {return (vect2(this->x - other.x, this->y - other.y));}
vect2   vect2::operator-(void) const {return (vect2(-this->x, -this->y));}                  //unary value
vect2   vect2::operator*(const int n)const {return (vect2(this->x * n, this->y * n));}
vect2&  vect2::operator++(void) {this->x++; this->y++; return *this;}
vect2   vect2::operator++(int) { vect2 tmp(*this); ++(*this); return (tmp);}
vect2&  vect2::operator--(void) {this->x--; this->y--; return *this;}
vect2   vect2::operator--(int) {vect2 tmp(*this); --(*this); return (tmp);}
bool    vect2::operator==(const vect2& other) const {return ((this->x == other.x) && (this->y == other.y));}
bool    vect2::operator!=(const vect2& other) const {return (!(*this == other));}
std::ostream&    operator<<(std::ostream& os, const vect2& v) {
    os << "{" << v.x << ":" << v.y << "}";
    return (os);
}
vect2    operator*(const int n, const vect2& other) {
    return (vect2(n * other.x, n * other.y));
}