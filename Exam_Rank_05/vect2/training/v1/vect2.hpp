#pragma once

#include <iostream>

class vect2{
    private:
        int _x;
        int _y;
    public:
        vect2(): _x(0), _y(0) {}
        vect2(const int x, const int y): _x(x), _y(y) {}
        vect2(const vect2& o): _x(o._x), _y(o._y) {}
        vect2& operator=(const vect2& o) {_x = o._x; _y = o._y; return (*this);}
        ~vect2(){}

        int& operator[](const int idx){return (idx == 0? _x : _y);}
        int operator[](const int idx) const {return (idx == 0? _x : _y);}

        vect2& operator++(void) {++_x; ++_y; return *this;}
        vect2& operator--(void) {--_x; --_y; return *this;}
        vect2 operator++(int) {vect2 tmp(*this); ++(*this); return (tmp);}        
        vect2 operator--(int) {vect2 tmp(*this); --(*this); return (tmp);}

        vect2& operator+=(const vect2& o) { _x += o._x; _y += o._y; return (*this);}
        vect2& operator-=(const vect2& o) { _x -= o._x; _y -= o._y; return (*this);}
        vect2& operator*=(const int n) { _x *= n; _y *= n; return (*this);}

        vect2 operator+(const vect2& o) const {return (vect2(_x + o._x, _y + o._y));}
        vect2 operator-(const vect2& o) const {return (vect2(_x - o._x, _y - o._y));}
        vect2 operator-(void) const {return (vect2(-_x, -_y));}
        vect2 operator*(const int n) const {return (vect2(_x * n, _y * n));}

        bool operator==(const vect2& o) const {return  ((_x == o._x) && (_y == o._y));}
        bool operator!=(const vect2& o) const {return (!(*this == o));}
        friend vect2 operator*(const int n, const vect2& v) {return (vect2(v * n));}
        friend std::ostream& operator<<(std::ostream& os, const vect2& v){return (os << "{" <<  v[0] << ", " << v[1] << "}");}

};