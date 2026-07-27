#pragma once

#include "print_utils.hpp"
#include <string>

// A tiny hand-rolled test runner: same idea as ASSERT_EQ in real test
// frameworks, just small enough to read in one screen.
class TestSuite {
    private:
        int _passed;
        int _failed;

    public:
        TestSuite() : _passed(0), _failed(0) {}

        // T and U can differ (e.g. comparing an int result against an int
        // literal) as long as `expected == result` and both are printable.
        template <typename T, typename U>
        void assertEqual(const T& expected, const U& result, const std::string& label) {
            if (expected == result) {
                _passed++;
                success("[PASS] ", label);
            } else {
                _failed++;
                error("[FAIL] ", label, " -- expected: ", expected, ", got: ", result);
            }
        }

        void summary() const {
            print("-----------------------------");
            if (_failed == 0)
                success(_passed, "/", _passed + _failed, " tests passed");
            else
                error(_passed, "/", _passed + _failed, " tests passed");
        }
};
