// Exhaustive test suite for bigint. Build:
//   c++ -Wall -Wextra -Werror -std=c++98 bigint.cpp tests.cpp -o tests
#include "bigint.hpp"
#include <sstream>
#include <cstdlib>

static int failed = 0;
static int total = 0;

static std::string str(const bigint& b) {
	std::ostringstream os;
	os << b;
	return os.str();
}

static void check(bool ok, const std::string& what) {
	++total;
	if (!ok) {
		++failed;
		std::cout << "FAIL: " << what << std::endl;
	}
}

static void eq(const bigint& b, const std::string& want, const std::string& what) {
	++total;
	if (str(b) != want) {
		++failed;
		std::cout << "FAIL: " << what << " -> got " << str(b) << ", want " << want << std::endl;
	}
}

// reference decimal addition on plain strings, deliberately naive
static std::string refadd(std::string a, std::string b) {
	std::string r;
	int carry = 0;
	for (size_t i = 0; i < a.size() || i < b.size() || carry; ++i) {
		int d = carry;
		if (i < a.size()) d += a[a.size() - 1 - i] - '0';
		if (i < b.size()) d += b[b.size() - 1 - i] - '0';
		r.insert(r.begin(), char('0' + d % 10));
		carry = d / 10;
	}
	size_t p = r.find_first_not_of('0');
	return p == std::string::npos ? "0" : r.substr(p);
}

static std::string randnum(unsigned& seed, size_t len) {
	std::string r;
	for (size_t i = 0; i < len; ++i) {
		seed = seed * 1103515245u + 12345u;
		r += char('0' + (seed >> 16) % 10);
	}
	size_t p = r.find_first_not_of('0');
	return p == std::string::npos ? "0" : r.substr(p);
}

int main() {
	std::cout << "--- construction / canonical form ---" << std::endl;
	bigint z;
	eq(z, "0", "default ctor is 0");
	eq(bigint(0), "0", "bigint(0)");
	eq(bigint(7), "7", "bigint(7)");
	eq(bigint(42), "42", "bigint(42)");
	eq(bigint(1337), "1337", "bigint(1337)");
	eq(bigint(2147483647), "2147483647", "bigint(INT_MAX)");
	eq(bigint(-5), "0", "negative clamps to 0 (unsigned type)");
	eq(bigint(std::string("0000")), "0", "string ctor strips to 0");
	eq(bigint(std::string("000123")), "123", "string ctor strips leading zeros");
	eq(bigint(std::string("100")), "100", "string ctor keeps trailing zeros");

	bigint src(999);
	bigint cpy(src);
	eq(cpy, "999", "copy ctor");
	bigint asg;
	asg = src;
	eq(asg, "999", "copy assignment");
	asg = asg;
	eq(asg, "999", "self assignment");
	src += 1;
	eq(cpy, "999", "copy is deep (source mutation does not leak)");
	eq(src, "1000", "source mutated independently");

	std::cout << "--- printing ---" << std::endl;
	eq(bigint(std::string("0")), "0", "zero prints as 0");
	eq(bigint(10), "10", "no leading zeros");
	eq(bigint(1000000), "1000000", "no leading zeros big");
	{
		std::ostringstream os;
		os << bigint(1) << "-" << bigint(2);
		check(os.str() == "1-2", "operator<< chains and returns the stream");
	}

	std::cout << "--- addition ---" << std::endl;
	eq(bigint(0) + bigint(0), "0", "0 + 0");
	eq(bigint(0) + bigint(42), "42", "0 + 42");
	eq(bigint(42) + bigint(0), "42", "42 + 0");
	eq(bigint(42) + bigint(21), "63", "42 + 21");
	eq(bigint(9) + bigint(1), "10", "carry out");
	eq(bigint(999) + bigint(1), "1000", "carry chain");
	eq(bigint(std::string("99999999999999999999")) + bigint(1), "100000000000000000000", "carry past 64 bits");
	eq(bigint(std::string("1")) + bigint(std::string("99999999999999999999")), "100000000000000000000", "short + long");
	eq(bigint(std::string("18446744073709551615")) + bigint(std::string("18446744073709551615")),
	   "36893488147419103230", "SIZE_MAX + SIZE_MAX");
	{
		bigint x(5);
		x += x;
		eq(x, "10", "self += (aliasing)");
		bigint y(std::string("99999999999999999999"));
		y += y;
		eq(y, "199999999999999999998", "self += with carry (aliasing)");
		bigint w(7);
		eq(w + w, "14", "self + self");
	}
	{
		bigint acc;
		for (int i = 1; i <= 100; ++i)
			acc += i;
		eq(acc, "5050", "sum 1..100");
	}
	{
		const bigint ca(42), cb(21);
		eq(ca + cb, "63", "operator+ is const-callable");
		bigint r = ca;
		r += cb;
		eq(r, "63", "+= from const operand");
		eq(ca, "42", "operator+ leaves lhs untouched");
	}
	eq(bigint(41) + 1, "42", "implicit int -> bigint on rhs");

	std::cout << "--- increment ---" << std::endl;
	{
		bigint b(21);
		eq(++b, "22", "pre-increment returns new value");
		eq(b, "22", "pre-increment mutated");
		eq(b++, "22", "post-increment returns old value");
		eq(b, "23", "post-increment mutated");
		bigint n(9);
		eq(++n, "10", "pre-increment carries");
		bigint m(std::string("999999999999999999999999"));
		eq(m++, "999999999999999999999999", "post-increment old value big");
		eq(m, "1000000000000000000000000", "post-increment carried");
		bigint p(0);
		eq(++p, "1", "0 -> 1");
		// chained pre-increment must operate on the same object
		bigint q(0);
		++(++q);
		eq(q, "2", "chained pre-increment");
	}

	std::cout << "--- digitshift ---" << std::endl;
	eq(bigint(42) << 3, "42000", "42 << 3 == 42000");
	eq(bigint(1337) >> 2, "13", "1337 >> 2 == 13");
	eq(bigint(42) << 0, "42", "shift left by 0");
	eq(bigint(42) >> 0, "42", "shift right by 0");
	eq(bigint(0) << 5, "0", "0 << 5 stays 0 (no leading zeros)");
	eq(bigint(0) >> 5, "0", "0 >> 5 stays 0");
	eq(bigint(1337) >> 4, "0", "shift right by exactly the length");
	eq(bigint(1337) >> 99, "0", "shift right past the length");
	eq(bigint(1) << 50, "100000000000000000000000000000000000000000000000000", "1 << 50");
	eq(bigint(std::string("100")) >> 2, "1", "trailing zeros consumed");
	{
		bigint d(1337);
		eq(d <<= 4, "13370000", "(d <<= 4)");
		eq(d >>= (const bigint)2, "133700", "(d >>= (const bigint)2)");
		eq(d, "133700", "compound shifts mutate");
		bigint b(23);
		eq((b << 10) + 42, "230000000042", "(b << 10) + 42");
		eq(b, "23", "operator<< leaves lhs untouched");
	}
	{
		const bigint cb(42);
		eq(cb << 2, "4200", "operator<< is const-callable");
		eq(cb >> 1, "4", "operator>> is const-callable");
		eq(bigint(42) << bigint(3), "42000", "shift count as bigint");
		eq(bigint(42) << bigint(std::string("3")), "42000", "shift count built from string");
	}
	{
		// shift then unshift round-trips
		bigint x(std::string("987654321"));
		bigint y = (x << 20) >> 20;
		check(y == x, "(x << 20) >> 20 == x");
	}
	{
		// shift by a count that overflows size_t must saturate, not wrap
		eq(bigint(1337) >> bigint(std::string("99999999999999999999999")), "0", ">> by absurd count is 0");
		eq(bigint(1337) >> bigint(std::string("4294967296")), "0", ">> by 2^32 is 0");
		eq(bigint(0) << bigint(std::string("99999999999999999999999")), "0", "0 << absurd count is 0");
	}
	{
		// self-aliasing shifts: the count is read from the operand being written
		bigint x(3);
		x <<= x;
		eq(x, "3000", "x <<= x");
		bigint y(2);
		y >>= y;
		eq(y, "0", "y >>= y");
		bigint w(12);
		eq(w << w, "12000000000000", "w << w");
	}

	std::cout << "--- comparison ---" << std::endl;
	{
		const bigint small(42), big(133700), same(42);
		check(small < big, "42 < 133700");
		check(!(big < small), "!(133700 < 42)");
		check(!(small < same), "!(42 < 42)");
		check(big > small, "133700 > 42");
		check(!(small > big), "!(42 > 133700)");
		check(small <= same, "42 <= 42");
		check(small <= big, "42 <= 133700");
		check(!(big <= small), "!(133700 <= 42)");
		check(small >= same, "42 >= 42");
		check(big >= small, "133700 >= 42");
		check(small == same, "42 == 42");
		check(!(small == big), "!(42 == 133700)");
		check(small != big, "42 != 133700");
		check(!(small != same), "!(42 != 42)");
	}
	// equal length, differing digits: must not fall back to length only
	check(bigint(199) < bigint(200), "199 < 200 (same length)");
	check(bigint(200) > bigint(199), "200 > 199 (same length)");
	check(!(bigint(999) < bigint(111)), "999 !< 111 (same length)");
	// different length: must not be lexicographic
	check(bigint(9) < bigint(10), "9 < 10 (lexicographic trap)");
	check(bigint(std::string("99999999")) < bigint(std::string("100000000")), "99999999 < 100000000");
	check(bigint(0) < bigint(1), "0 < 1");
	check(bigint(0) == bigint(std::string("0000")), "0 == 0000");
	check(bigint(0) <= bigint(0) && bigint(0) >= bigint(0), "0 <=> 0");
	check(bigint(42) == 42, "implicit int on rhs of ==");
	{
		// strict weak ordering sanity across a spread of values
		const char* v[] = {"0", "1", "9", "10", "11", "99", "100", "12345678901234567890",
		                   "12345678901234567891", "99999999999999999999999999"};
		for (int i = 0; i < 10; ++i)
			for (int j = 0; j < 10; ++j) {
				bigint x((std::string(v[i]))), y((std::string(v[j])));
				check((x < y) == (i < j), "ordering consistency");
				check((x == y) == (i == j), "equality consistency");
				check((x <= y) == (i <= j), "<= consistency");
				check((x > y) == (i > j), "> consistency");
				check((x >= y) == (i >= j), ">= consistency");
				check((x != y) == (i != j), "!= consistency");
			}
	}

	std::cout << "--- arbitrary precision (fuzz vs reference) ---" << std::endl;
	{
		unsigned seed = 42;
		for (int it = 0; it < 3000; ++it) {
			size_t la = 1 + (it * 7) % 120;
			size_t lb = 1 + (it * 13) % 120;
			std::string a = randnum(seed, la), b = randnum(seed, lb);
			bigint x((a)), y((b));
			std::string want = refadd(a, b);
			if (str(x + y) != want) {
				std::cout << "FAIL: " << a << " + " << b << " -> " << str(x + y) << ", want " << want << std::endl;
				++failed;
			}
			++total;
			// commutativity
			check(str(y + x) == want, "addition is commutative");
			// comparison against reference string compare
			bool wantlt = (a.size() != b.size()) ? a.size() < b.size() : a < b;
			check((x < y) == wantlt, "comparison matches reference");
		}
	}
	{
		// 1000-digit accumulation: 10^999 built by shifting, then walked
		bigint big = bigint(1) << 999;
		check(str(big).size() == 1000, "1 << 999 has 1000 digits");
		check(str(big)[0] == '1', "1 << 999 starts with 1");
		bigint back = big >> 999;
		check(back == bigint(1), "(1 << 999) >> 999 == 1");
		bigint nines;
		for (int i = 0; i < 999; ++i)
			nines = (nines << 1) + 9;
		check(str(nines).size() == 999, "999 nines built");
		check(nines + 1 == big, "999 nines + 1 == 10^999");
		check(nines < big, "999 nines < 10^999");
	}
	{
		// fibonacci 300 = 222232244629420445529739893461909967206666939096499764990979600
		bigint p(0), q(1);
		for (int i = 0; i < 300; ++i) {
			bigint t = p + q;
			p = q;
			q = t;
		}
		eq(p, "222232244629420445529739893461909967206666939096499764990979600", "fib(300)");
	}
	{
		// 2^256 by repeated doubling
		bigint p(1);
		for (int i = 0; i < 256; ++i)
			p += p;
		eq(p, "115792089237316195423570985008687907853269984665640564039457584007913129639936", "2^256");
	}

	std::cout << std::endl;
	if (failed)
		std::cout << failed << " / " << total << " checks FAILED" << std::endl;
	else
		std::cout << "all " << total << " checks passed" << std::endl;
	return failed ? 1 : 0;
}
