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
        ~vect2();

        int     operator[](const int idx) const;
        int&    operator[](const int idx);

        vect2&   operator=(const vect2& other);
        vect2&   operator+=(const vect2& other);
        vect2&   operator-=(const vect2& other);
        vect2&   operator*=(const int n);

        vect2   operator+(const vect2& other)const;
        vect2   operator-(const vect2& other)const;
        vect2   operator-(void) const;                  //unary value
        vect2   operator*(const int n)const;

        vect2&   operator++(void);
        vect2   operator++(int);
        vect2&   operator--(void);
        vect2   operator--(int);

        bool   operator!=(const vect2& other) const;
        bool   operator==(const vect2& other) const;

        friend std::ostream& operator<<(std::ostream& os, const vect2& v);
        friend vect2   operator*(const int n, const vect2& other);
};

