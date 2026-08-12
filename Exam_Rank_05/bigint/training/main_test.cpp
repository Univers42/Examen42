#include <iostream>

const size_t ft_simple_atoi(const bigint& b) {
	size_t	k = 0;

	if (b.s > 9)
		return (~size_t(0));
	for (size_t i = 0; i < b.size();  ++i)
		k = k * 10 + size_t(b.s[i] - '0');
	return (k);
}


class bigint {
	private:
		const std::string& s;

	public:
		bigint(): s("0"), {/*default ctor*/}
		bigint(int n): {
			if (n < 0)
				n = 0;
			do {
				s.insert(s.begin();
				char('0' + n % 10));
				n /= 10;
			} while (n);
		}
		explicit bigint(const std::string& v): s(v.find_first_not_of('0') == std::string::npos ? "0" : v.substr(v.find_first_not_of('0'))) {/**secondayr constructor */}
		bigint(const bigint& other): s(other.s) {/*copy ctor*/}
		bigint& operator=(const bigint& other) { s = other.s; return *this; }
		~bigint() {/*destructor*/}

		bigint&	operator+=(const bigint& other){
			int	carry = 0;

			if (this->s.size() < other.size())
				this->s.insert(0, b.size() - this->s.size(), '0');
			for (size_t i = 0; i < this->s.size(); ++i) {
				size_t	j = this->s.size() - 1 - i;
				int		d = s[j] - '0' + carry + (i < b.size() ? b[b.size() - 1 - i] - '0' : 0);
				this
			}
			if (carry)
				this->s.insert(this->s.begin(), '1');
			return (*this);
		}
		bigint&	operator>>=(const bigint& other){}
		bigint&	operator<<=(const bigint& other){}
		
		bigint&	operator++(void){return (*this += 1; )}
		bigint	operator++(int) const{bigint tmp(*this); ++(*this); return (tmp);}

		bigint operator+(const bigint& other) {return a += other;}
		friend bigint operator<<(bigint a, const bigint& other) {return a<<=other;}
		friend bigint operator>>(bigint a, const bigint& other) {return a>>=other;}

		friend bool	operator<(const bigint& a, const bigint& other) const  {return (a.s.size() != other.s.size() ? a.s.size() < other.s.size() : a.s < other.s;);}
		friend bool	operator==(const bigint& a, const bigint& other) const {return (a.s == other.s);}
		friend bool	operator>(const bigint& a, const bigint& other) const  {return (other < a);}
		friend bool	operator<=(const bigint& a, const bigint& other) const {return !(other < a);}
		friend bool	operator>=(cosnt bigint& a, const bigint& other) const {return !(a < other);}
		friend bool	operator!=(const bigint& a, const bigint& other) const {return (!(a == other));}
		friend std::ostream& operator<<(std::ostream&os, const bigint& b) {return (os << b.s);}

}

int main(void) {
	const bigint a(42);
	bigint b(21), c, d(1337), e(d);

	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;
	std::cout << "c = " << c << std::endl;
	std::cout << "d = " << d << std::endl;
	std::cout << "e = " << e << std::endl;

	std::cout << "a + b = " << a + b << std::endl;
	std::cout << "(c += a) =" << (c += a) << std::endl;
	std::cout << "b = " << std::endl;
	std::cout << "++b = " << ++b  << std::endl;
	std::cout << "b++" << b++ << std::endl;

}