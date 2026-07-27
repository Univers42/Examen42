#include "print_utils.hpp"

int main() {
    print("plain print, no color, auto newline: ", 42);

    success("this passed: ", 1 + 1, " == 2");
    error("this failed: expected ", 4, " got ", 5);
    warn("heads up, x = ", 3.14);
    info("just some info, v = ", 7);

    std::cout << kolor::bold << kolor::magenta << "manual chaining still works"
               << kolor::reset << std::endl;
}
