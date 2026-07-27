#pragma once
#include <iostream>
#include <unistd.h>

namespace kolor {
    inline bool isTTY() {return isatty(fileno(stdout));}
    inline std::ostream& reset(std::ostream& os) { return isTTY() ? os <<  "\033[0m" : os;}
    inline std::ostream& bold(std::ostream& os) { return isTTY() ? os << "\033[1m" : os;}
    inline std::ostream& dim(std::ostream& os) { return isTTY() ? os << "\033[2m" : os;}
    inline std::ostream& red(std::ostream& os) { return isTTY() ? os << "\033[31m" : os;}
    inline std::ostream& green(std::ostream& os) { return isTTY() ? os << "\033[32m" : os;}
    inline std::ostream& yellow(std::ostream& os) { return isTTY() ? os << "\033[33m" : os;}
    inline std::ostream& blue(std::ostream& os) { return isTTY() ? os << "\033[34m" : os;}
    inline std::ostream& magenta(std::ostream& os) { return isTTY() ? os << "\033[35m" : os;}
    inline std::ostream& cyan(std::ostream& os) { return isTTY() ? os << "\033[36m" : os;}

}

template <typename... Args>
void print(const Args&... args) {
    (std::cout << ... << args) << '\n';
}

template <typename... Args>
void success(const Args&... args) {
    std::cout << kolor::green; print(args...); std::cout << kolor::reset; 
}

template <typename... Args>
void error(const Args&... args) {
    std::cout << kolor::red; print(args...); std::cout << kolor::reset;
}

template <typename... Args>
void warn(const Args&... args) {
    std::cout << kolor::yellow; print(args...); std::cout << kolor::reset;
}

template <typename... Args>
void info(const Args&... args) {
    std::cout << kolor::cyan; print(args...); std::cout << kolor::reset;
}
