#pragma once
#include <iostream>

class vect2;

// 1. Trait Declarations
template <typename T>
struct vect2_trait {
    static int get_x(const T& val) { return static_cast<int>(val); }
    static int get_y(const T& val) { return static_cast<int>(val); }
};

template <>
struct vect2_trait<vect2> {
    static int get_x(const vect2& val);
    static int get_y(const vect2& val);
};

// 2. Class Declaration
class vect2 {
    private:
        int _x;
        int _y;

        friend struct vect2_trait<vect2>;

    public:
        vect2(void);
        vect2(int, int);
        vect2(const vect2&);
        vect2& operator=(const vect2&); // Fixed: specified vect2 instead of unknown T
        ~vect2(void);

        int  operator[](const int) const;
        int& operator[](const int);

        // Template operators must be defined in the header
        template <typename T>
        vect2& operator+=(const T& other) {
            _x += vect2_trait<T>::get_x(other);
            _y += vect2_trait<T>::get_y(other);
            return (*this);
        }

        template <typename T>
        vect2& operator*=(const T& other) {
            _x *= vect2_trait<T>::get_x(other);
            _y *= vect2_trait<T>::get_y(other);
            return (*this);
        }

        template <typename T>
        vect2& operator-=(const T& other) {
            _x -= vect2_trait<T>::get_x(other);
            _y -= vect2_trait<T>::get_y(other); // Fixed: double colon ::
            return (*this);
        }

        vect2& operator++(void);
        vect2  operator++(int);
        vect2  operator--(int);
        vect2& operator--(void);

        friend std::ostream& operator<<(std::ostream&, const vect2&);
};

// 3. Trait Specialization Definitions (must be visible to header templates)
inline int vect2_trait<vect2>::get_x(const vect2& val) { return val._x; }
inline int vect2_trait<vect2>::get_y(const vect2& val) { return val._y; }