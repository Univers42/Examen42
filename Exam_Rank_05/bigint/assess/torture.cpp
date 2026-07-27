// Overload-resolution torture: every expression form a grader main might use.
// Success criterion is that this COMPILES (and prints the right things).
#include "../bigint.hpp"
#include <iostream>

int main() {
	const bigint a(42);
	bigint b(21), c, d(1337), e(d);

	// ostream must never be confused with the digitshift operator<<
	std::cout << a << " " << 42 << " " << "lit" << " " << 4.2 << " " << 'x'
	          << " " << true << " " << (void*)0 << std::endl;
	std::cout << b << 10 << std::endl;          // (cout << b) << 10, not cout << (b << 10)
	std::cout << (b << 10) << std::endl;

	// symmetric operands
	std::cout << a + b << " " << a + 1 << " " << 1 + a << " " << 1 + bigint(1) << std::endl;
	std::cout << (a < b) << (1 < a) << (a < 100) << (a == 42) << (42 == a) << (42 != a) << std::endl;

	// shifts both ways, with int / bigint / const bigint counts
	std::cout << (a << 2) << " " << (a << bigint(2)) << " " << (a << (const bigint)2) << " "
	          << (2 << a) << std::endl;
	std::cout << (d >> 2) << " " << (d >> bigint(2)) << " " << (d >> (const bigint)2) << std::endl;

	// compound forms, chained
	c += a;
	c += 1;
	c <<= 2;
	c >>= 1;
	std::cout << c << std::endl;
	std::cout << ((c += a) += b) << std::endl;
	std::cout << (((c <<= 1) >>= 1) += 0) << std::endl;

	// increments in every position (one per statement: argument evaluation
	// order inside a single << chain is unspecified before C++17)
	std::cout << ++b << " ";
	std::cout << b++ << " ";
	std::cout << b << " ";
	std::cout << ++(++b) << std::endl;

	// copy / assign / const correctness
	bigint f = a, g;
	g = f = b;
	const bigint h(g);
	std::cout << f << " " << g << " " << h << " " << e << std::endl;
	std::cout << h + h << " " << (h < h) << " " << (h << 1) << std::endl;

	// temporaries and nesting
	std::cout << ((bigint(1) << 20) + (bigint(2) << 10) + 3) << std::endl;
	std::cout << (bigint(bigint(bigint(9))) + bigint()) << std::endl;
	return 0;
}
