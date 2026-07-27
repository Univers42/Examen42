#include "print.hpp"
#include "vect2.hpp"
#include <string>

// A tiny hand-rolled test runner: same idea as ASSERT_EQ in real test
// frameworks, just small enough to read in one screen. It leans on vect2's
// own operator== (to compare) and operator<< (to print), so the same code
// works for vect2, int and bool.
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
                success("[PASS] ", label, " = ", result);
            } else {
                _failed++;
                error("[FAIL] ", label, " -- expected: ", expected, ", got: ", result);
            }
        }

        void section(const std::string& title) const {
            print("");
            info("== ", title, " ==");
        }

        int summary() const {
            print("-----------------------------");
            if (_failed == 0)
                success(_passed, "/", _passed + _failed, " tests passed");
            else
                error(_passed, "/", _passed + _failed, " tests passed");
            return (_failed == 0 ? 0 : 1);
        }
};

int main() {
    TestSuite st;

    st.section("Construction & accessors");
    {
        vect2 d;
        st.assertEqual(vect2(0, 0), d, "default vect2()");
        st.assertEqual(0, d[0], "default [0]");
        st.assertEqual(0, d[1], "default [1]");

        vect2 v(1, 2);
        st.assertEqual(vect2(1, 2), v, "vect2(1,2)");
        st.assertEqual(1, v[0], "v[0]");
        st.assertEqual(2, v[1], "v[1]");

        const vect2 c(v);
        st.assertEqual(vect2(1, 2), c, "copy ctor (const)");

        vect2 ci = v;
        st.assertEqual(vect2(1, 2), ci, "copy-init (=)");

        st.assertEqual(vect2(-3, -7), vect2(-3, -7), "negative components");
    }

    st.section("operator[] read (const & non-const)");
    {
        vect2 v(4, 9);
        st.assertEqual(4, v[0], "v[0]");
        st.assertEqual(9, v[1], "v[1]");

        const vect2 c(4, 9);
        st.assertEqual(4, c[0], "const c[0]");
        st.assertEqual(9, c[1], "const c[1]");
    }

    st.section("operator[] write");
    {
        vect2 v;
        v[0] = 10;
        v[1] = 20;
        st.assertEqual(vect2(10, 20), v, "after v[0]=10; v[1]=20");
        st.assertEqual(10, v[0], "v[0] read back");
        st.assertEqual(20, v[1], "v[1] read back");
    }

    st.section("Assignment");
    {
        vect2 a(56, 32);
        vect2 b;
        b = a;
        st.assertEqual(vect2(56, 32), b, "b = a");

        vect2 x, y, z(5, 6);
        x = y = z;                                  // chained assignment
        st.assertEqual(vect2(5, 6), x, "x = y = z  (x)");
        st.assertEqual(vect2(5, 6), y, "x = y = z  (y)");

        vect2 s(7, 9);
        vect2& alias = s;
        s = alias;                                  // self-assignment (via alias)
        st.assertEqual(vect2(7, 9), s, "self-assignment unchanged");
    }

    st.section("Arithmetic (+  -  unary-  *scalar)");
    {
        vect2 a(56, 32), b(1, 2);
        st.assertEqual(vect2(57, 34), a + b, "a + b");
        st.assertEqual(vect2(55, 30), a - b, "a - b");
        st.assertEqual(vect2(-3, 4), -vect2(3, -4), "unary -v");
        st.assertEqual(vect2(3, -4), -(-vect2(3, -4)), "double negation -(-v)");
        st.assertEqual(vect2(6, 8), vect2(3, 4) * 2, "v * 2");
        st.assertEqual(vect2(9, 12), 3 * vect2(3, 4), "3 * v (free op)");
        st.assertEqual(vect2(0, 0), vect2(3, 4) * 0, "v * 0");
        st.assertEqual(vect2(-3, -4), vect2(3, 4) * -1, "v * -1");
        st.assertEqual(vect2(56, 32), a, "operand a unchanged");
        st.assertEqual(vect2(1, 2), b, "operand b unchanged");
    }

    st.section("Increment / decrement");
    {
        vect2 v(1, 2);
        st.assertEqual(vect2(1, 2), v++, "v++ returns OLD");
        st.assertEqual(vect2(2, 3), v, "state after v++");
        st.assertEqual(vect2(3, 4), ++v, "++v returns NEW");
        st.assertEqual(vect2(3, 4), v, "state after ++v");
        st.assertEqual(vect2(3, 4), v--, "v-- returns OLD");
        st.assertEqual(vect2(2, 3), v, "state after v--");
        st.assertEqual(vect2(1, 2), --v, "--v returns NEW");
        st.assertEqual(vect2(1, 2), v, "state after --v");

        vect2 w(0, 0);
        ++(++w);                                    // pre-inc returns a ref -> chainable
        st.assertEqual(vect2(2, 2), w, "++(++w) double pre-inc");
    }

    st.section("Compound assignment");
    {
        vect2 v(10, 20);
        st.assertEqual(vect2(11, 22), (v += vect2(1, 2)), "(v += (1,2)) returns");
        st.assertEqual(vect2(11, 22), v, "state after +=");
        st.assertEqual(vect2(10, 20), (v -= vect2(1, 2)), "(v -= (1,2)) returns");
        st.assertEqual(vect2(10, 20), v, "state after -=");
        st.assertEqual(vect2(30, 60), (v *= 3), "(v *= 3) returns");
        st.assertEqual(vect2(30, 60), v, "state after *=");

        vect2 a(2, 3), b(1, 2);
        a += a += b;                                // inner a=(3,5); outer a += a => (6,10)
        st.assertEqual(vect2(6, 10), a, "a += a += b");

        vect2 c(0, 0);
        (c += vect2(1, 1)) += vect2(2, 2);          // += returns ref -> chainable
        st.assertEqual(vect2(3, 3), c, "(c += ) += chaining");
    }

    st.section("Comparison (==  !=)");
    {
        vect2 a(1, 2), b(1, 2), cx(1, 3), d(2, 2);
        st.assertEqual(true,  (a == b),  "equal ==");
        st.assertEqual(false, (a == d),  "x differs ==");
        st.assertEqual(false, (a == cx), "y differs ==");
        st.assertEqual(true,  (a == a),  "self ==");
        st.assertEqual(false, (a != b),  "equal !=");
        st.assertEqual(true,  (a != cx), "differ !=");
    }

    st.section("Integration (full subject-main trace)");
    {
        vect2       v1;
        vect2       v2(1, 2);
        const vect2 v3(v2);
        vect2       v4 = v2;

        st.assertEqual(vect2(1, 2), v4++, "v4++ OLD");
        st.assertEqual(vect2(3, 4), ++v4, "++v4 NEW");
        st.assertEqual(vect2(3, 4), v4--, "v4-- OLD");
        st.assertEqual(vect2(1, 2), --v4, "--v4 NEW");

        v2 += v3;           st.assertEqual(vect2(2, 4),     v2, "v2 += v3");
        v1 -= v2;           st.assertEqual(vect2(-2, -4),   v1, "v1 -= v2");
        v2 = v3 + v3 * 2;   st.assertEqual(vect2(3, 6),     v2, "v2 = v3 + v3*2");
        v2 = 3 * v2;        st.assertEqual(vect2(9, 18),    v2, "v2 = 3 * v2");
        v2 += v2 += v3;     st.assertEqual(vect2(20, 40),   v2, "v2 += v2 += v3");
        v1 *= 42;           st.assertEqual(vect2(-84, -168), v1, "v1 *= 42");
        v1 = v1 - v1 + v1;  st.assertEqual(vect2(-84, -168), v1, "v1 = v1 - v1 + v1");

        st.assertEqual(vect2(-20, -40), -v2, "-v2");
        st.assertEqual(-168, v1[1], "v1[1] before write");
        v1[1] = 12;
        st.assertEqual(12, v1[1], "v1[1] after write");
        st.assertEqual(2, v3[1], "v3[1] (const)");
        st.assertEqual(false, (v1 == v3), "v1 == v3");
        st.assertEqual(true,  (v1 == v1), "v1 == v1");
        st.assertEqual(true,  (v1 != v3), "v1 != v3");
        st.assertEqual(false, (v1 != v1), "v1 != v1");
    }

    return (st.summary());
}
