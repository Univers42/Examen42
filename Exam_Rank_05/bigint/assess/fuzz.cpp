// Cross-check driver: reads "A B K" lines, prints results for python to diff.
#include "../bigint.hpp"
#include <string>

int main() {
	std::string a, b, k;
	while (std::cin >> a >> b >> k) {
		bigint x((a)), y((b)), n((k));
		std::cout << (x + y) << ' ' << (x << n) << ' ' << (x >> n) << ' '
		          << (x < y) << (x <= y) << (x > y) << (x >= y) << (x == y) << (x != y) << '\n';
	}
	return 0;
}
