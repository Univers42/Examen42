#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <iostream>
#include <string>

// Arbitrary precision unsigned integer. Digits are stored in base 10, most
// significant first, always normalized: no leading zero, "0" for zero.
class bigint {
	std::string s;

	//inside any member function s is a shorthand of this->. The compiler resolve the name by looking at the enclosign class scope
public:
	bigint() : s("0") {}
	bigint(int n) : s() { if (n < 0) n = 0; do { s.insert(s.begin(), char('0' + n % 10)); n /= 10; } while (n); }
	explicit bigint(const std::string& v)
		: s(v.find_first_not_of('0') == std::string::npos ? "0" : v.substr(v.find_first_not_of('0'))) {/** secundary constructor */}
	bigint(const bigint& o) : s(o.s) {}
	bigint& operator=(const bigint& o) { s = o.s; return *this; }
	~bigint() {}

	bigint& operator+=(const bigint& o);
	bigint& operator<<=(const bigint& k);
	bigint& operator>>=(const bigint& k);

	bigint& operator++() { return *this += 1; }
	bigint operator++(int) { bigint t(*this); *this += 1; return t; }

	friend bigint operator+(bigint a, const bigint& b) { return a += b; }
	friend bigint operator<<(bigint a, const bigint& k) { return a <<= k; }
	friend bigint operator>>(bigint a, const bigint& k) { return a >>= k; }

	friend bool operator<(const bigint& a, const bigint& b)
		{ return a.s.size() != b.s.size() ? a.s.size() < b.s.size() : a.s < b.s; }
	friend bool operator==(const bigint& a, const bigint& b) { return a.s == b.s; }
	friend bool operator>(const bigint& a, const bigint& b) { return b < a; }
	friend bool operator<=(const bigint& a, const bigint& b) { return !(b < a); }
	friend bool operator>=(const bigint& a, const bigint& b) { return !(a < b); }
	friend bool operator!=(const bigint& a, const bigint& b) { return !(a == b); }

	friend std::ostream& operator<<(std::ostream& os, const bigint& b) { return os << b.s; }
};

#endif
