#pragma once
#include <iostream>
#include <sstream>
#include <string>

// Minimal C++98 test harness. Header-only, no globals in a .cpp: the counters
// live in function-local statics so this header can be included from several
// translation units without an ODR problem.
namespace vtest {

    inline int&  passed()  { static int n = 0; return n; }
    inline int&  failed()  { static int n = 0; return n; }
    inline bool& verbose() { static bool v = false; return v; }

    inline const char* green() { return "\033[32m"; }
    inline const char* red()   { return "\033[1;31m"; }
    inline const char* cyan()  { return "\033[36m"; }
    inline const char* off()   { return "\033[0m"; }

    // Renders anything streamable to a string, so failures can print both the
    // expected and the obtained value in the same format.
    template <typename T>
    std::string str(const T& v) { std::ostringstream os; os << v; return (os.str()); }

    inline void section(const std::string& title) {
        std::cout << "\n" << cyan() << "== " << title << " ==" << off() << "\n";
    }

    // expected and result may be different types (int vs long, vect2 vs vect2)
    // as long as `expected == result` compiles and both are printable.
    template <typename T, typename U>
    bool check(const T& expected, const U& result, const std::string& label) {
        if (expected == result) {
            passed()++;
            if (verbose())
                std::cout << green() << "  [ok] " << off() << label << " = " << str(result) << "\n";
            return (true);
        }
        failed()++;
        std::cout << red() << "  [KO] " << off() << label
                  << "  expected: " << str(expected)
                  << "  got: " << str(result) << "\n";
        return (false);
    }

    inline bool assertTrue(bool cond, const std::string& label) {
        return (check(true, cond, label));
    }

    inline int summary(const std::string& what) {
        int p = passed();
        int f = failed();
        std::cout << "\n-----------------------------\n";
        if (f == 0)
            std::cout << green() << p << "/" << p + f << " " << what << " passed" << off() << "\n";
        else
            std::cout << red() << p << "/" << p + f << " " << what << " passed ("
                      << f << " failed)" << off() << "\n";
        return (f == 0 ? 0 : 1);
    }
}
