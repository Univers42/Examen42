#pragma once
#include <iostream>

class vect2 {
    private:
        int x;
        int y;
    public:
        vect2();
        vect2(const int x, const int y);
        vect2(const vect2& other);
        vect2& operator=(const vect2& other);
        ~vect2();

        int     operator[](const int index) const;
        int&    operator[](const int index);
        vect2   operator+(const vect2& other) const;
        vect2   operator-(const vect2& other) const;
        vect2   operator*(const int n) const;
        vect2   operator-(void) const;

        vect2&  operator++(void);
        vect2   operator++(int);
        vect2&  operator--(void);
        vect2   operator--(int);

        vect2&  operator+=(const vect2& other);
        vect2&  operator-=(const vect2& other);
        vect2&  operator*=(const int n);

        bool    operator!=(const vect2& other) const;
        bool    operator==(const vect2& other) const;
        friend std::ostream& operator<<(std::ostream& os, const vect2& other) {
            os << "{" << other.x << ", "  << other.y << "}";
            return (os);
        }
        friend vect2    operator*(const int n, const vect2& other) {
            return (vect2(n * other.x, n * other.y));
        }
};

