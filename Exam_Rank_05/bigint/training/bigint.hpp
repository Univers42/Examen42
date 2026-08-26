#pragma once
#include <iostream>

class bigint {
    private:
        std::string _s;
    public:
        bigint(): _s("0") {}
        explicit bigint(const int n): _s(if (n < 0) n = 0; do {_s.insert(_s.begin(), char('0' + n % 10)); n /= 10;} while (n);){}
        explicit bigint(const std::string& v): _s(v.find_first_not_of('0') == std::string::npos ? "0" : v.substr(v.find_first_not_of('0'))){}
        bigint(const bigint& o): _s(o._s) {}
        bigint& operator=(const bigint& o){_s = o._s; return *this;}
        ~bigint(){}

        static size_t count(std::string v) {
            size_t k  = 0;
            if (v.size() > 9)
                return (~size_t(0));
            else
            {
                for (size_t i = 0; i < v.size(); ++i)
                    k = k * 10 + size_t(v[i] -  '0');
            }
            return k;
        }

        bigint& operator+=(const bigint& o){
            const std::string b = o.s;
            const size_t nb = b.size();
            const size_t lo = _s.size();
            if (lo < nb)
                _s.insert(0, b, 0, nb - lo);
            else
                lo = nb;
            const size_t n = _s.size();
            const char *bq = b.data() + n - 1;
            char *sq = &_s[0] + n - 1;
            size_t i = 0;
            int carry = 0;
        
            for (; i < lo; ++i, --sq, --bq) {
                
            }
            for (; carry && i < )
        }
        
        bigint& operator<<=(const bigint& k){
            size_t n = count(k.s);
            if (_s != "0")
                _s.append(n, '0');
            return *this;
        }

        bigint& operator>>=(const bigint& k){
            size_t n = count(k.s);
            size_t len = _s.size();
            
            if (n >= len)
                _s = "0";
            else
                _s.erase(len - n);
            return *this;
        }
        
        bigint& operator++(void){return (*this += 1);}
        bigint& operator++(int){bigint tmp(*this), *this += 1; return (tmp);}

        friend bigint operator+(bigint a, const bigint& b){return (a += b);}
        friend bigint operator<<(bigint a, const bigint& b){return (a <<= b);}
        friend bigint operator>>(bigint a, const bigint& b){return (a >>= b);}
        
        friend bool operator <(const bigint& a, const bigint& b){
            a.s.size() != b.s.size() ? a.s.size() < b.s.size() : a.s < b.s;
        }
        friend bool operator ==(const bigint& a, const bigint& b){return (a.s == b.s);}
        friend bool operator <=(const bigint& a, const bigint& b){return !(b < a)}
        friend bool operator >=(const bigint& a, const bigint& b){return !(a < b);}
        friend bool operator !=(const bigint& a, const bigint& b){return (!(a == b));}
        friend bool operator >(const bigint& a, const bigint& b){return (b < a);}
}