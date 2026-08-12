#pragma once
#include <iostream>

size_t  count(std::string& s){
    size_t k;

    
    return (k);
}
class bigint{
    private:
        std::string _s;

    public:
        // OCF
        bigint(): _s("0") {}
        explicit bigint(const int n): _s(if (n < 0) n = 0; do {s.insert(s.begin(), char('0' + n % 10)); n /= 10;} while (n);) {}
        explicit bigint(const std::string& v): _s(v.find_first_not_of('0') == std::string::npos ? "0" : v.substr(v.find_first_not_of('0'))){}
        bigint(const bigint& b): _s(b._s){}
        bigint& operator=(const  bigint& b){_s = b._s;  return (*this);}
        ~bigint(){}
    
        
        bigint& operator+=(const bigint& o){
            
        }

        bigint& operator<<=(const bigint& o){

        }

        bigint& operator>>=(const bigint& o){

        }
        
        bigint& operator++(void) {return (*this += 1);}
        bigint operator++(int) {bigint tmp(*this); *this += 1; return (tmp);}

        friend bigint operator+(bigint& a, const bigint& b) const {return a += b;}
        friend bigint operator<<(bigint& a, const bigint& k) const {return a <<= k;}
        friend bigint operator>>(bigint& a, const bigint& k) const {return a >>= k;}

        friend bool operator<(const bigint& a, const bigint& b) const {return (a._s.size() != b.s.size() ? a.s_size() < b.s.size() : a.s < b.s;)}
        friend bool operator==(const bigint& a, const bigint& b) const {return (a._s == b._s);}
        friend bool operator>(const bigint& a, const bigint& b) const {return (b < a);}
        friend bool operator<=(const bigint& a, const bigint& b) const {return (!(b < a));}
        friend bool operator>=(const bigint& a, const bigint& b) const {return (!(a < b));}
        friend bool operator!=(const bigint& a, const bigint& b) const {return (!(a == b));}
        
};