#pragma once

#include <iostream>
#include <unistd.h>

// ---------------------------------------------------------------------------
// kolor:: — stream "decorators". Each function wraps std::ostream, writes an
// ANSI escape code, and hands the same stream back so it slots straight into
// a << chain, exactly like std::hex or std::setw do.
//
//   std::cout << kolor::red << "oops" << kolor::reset << std::endl;
// ---------------------------------------------------------------------------
namespace kolor {
    inline bool isTTY() { return isatty(fileno(stdout)); }

    inline std::ostream& reset(std::ostream& os)   { return isTTY() ? os << "\033[0m"  : os; }
    inline std::ostream& bold(std::ostream& os)    { return isTTY() ? os << "\033[1m"  : os; }
    inline std::ostream& dim(std::ostream& os)     { return isTTY() ? os << "\033[2m"  : os; }

    inline std::ostream& red(std::ostream& os)     { return isTTY() ? os << "\033[31m" : os; }
    inline std::ostream& green(std::ostream& os)   { return isTTY() ? os << "\033[32m" : os; }
    inline std::ostream& yellow(std::ostream& os)  { return isTTY() ? os << "\033[33m" : os; }
    inline std::ostream& blue(std::ostream& os)    { return isTTY() ? os << "\033[34m" : os; }
    inline std::ostream& magenta(std::ostream& os) { return isTTY() ? os << "\033[35m" : os; }
    inline std::ostream& cyan(std::ostream& os)    { return isTTY() ? os << "\033[36m" : os; }
}

// ---------------------------------------------------------------------------
// print(a, b, c, ...) streams every argument, in order, then a newline.
//   std::cout << "v1: " << v1 << std::endl;   ->   print("v1: ", v1);
// ---------------------------------------------------------------------------
template <typename... Args>
void print(const Args&... args) {
    (std::cout << ... << args) << '\n';
}

// success/error/warn/info — each one decorates print() with a color, then
// resets it. Same idea as kolor:: manipulators, one level up.
template <typename... Args>
void success(const Args&... args) {
    std::cout << kolor::green; print(args...); std::cout << kolor::reset;
}

template <typename... Args>
void error(const Args&... args) {
    std::cout << kolor::bold << kolor::red; print(args...); std::cout << kolor::reset;
}

template <typename... Args>
void warn(const Args&... args) {
    std::cout << kolor::yellow; print(args...); std::cout << kolor::reset;
}

template <typename... Args>
void info(const Args&... args) {
    std::cout << kolor::cyan; print(args...); std::cout << kolor::reset;
}
