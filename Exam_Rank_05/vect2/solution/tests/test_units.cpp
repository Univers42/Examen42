#include "../vect2.hpp"
#include "vtest.hpp"
#include <climits>
#include <sstream>
#include <string>

using vtest::check;
using vtest::assertTrue;
using vtest::section;

// Self-assignment (s = s, s -= s) is a case under test here, not a mistake, so
// clang's warning about it is noise in this file only.
#ifdef __clang__
# pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif


static std::string streamed(const vect2& v) {
    std::ostringstream os;
    os << v;
    return (os.str());
}

static std::string canonical(const vect2& v) {
    std::ostringstream os;
    os << "{" << v[0] << ", " << v[1] << "}";
    return (os.str());
}

static void checkFormat(const vect2& v, const std::string& label) {
    check(canonical(v), streamed(v), label);
}

// ---------------------------------------------------------------------------

static void testConstruction() {
    section("Construction");

    vect2 d;
    check(0, d[0], "default ctor x == 0");
    check(0, d[1], "default ctor y == 0");

    vect2 v(1, 2);
    check(1, v[0], "vect2(1,2)[0]");
    check(2, v[1], "vect2(1,2)[1]");

    // Order matters: (1,2) must not come back as (2,1).
    assertTrue(!(vect2(1, 2) == vect2(2, 1)), "components are not swapped");

    vect2 neg(-3, -7);
    check(-3, neg[0], "negative x");
    check(-7, neg[1], "negative y");

    vect2 mix(-1, 1);
    check(-1, mix[0], "mixed sign x");
    check(1, mix[1], "mixed sign y");

    // Temporaries are usable directly (needed by the subject expression
    // vect2(2,2) * 2 == vect2(4,4)).
    assertTrue(vect2(2, 2) * 2 == vect2(4, 4), "subject: vect2(2,2)*2 == vect2(4,4)");
}

static void testCopy() {
    section("Copy semantics");

    vect2 src(5, 6);
    vect2 cpy(src);
    check(vect2(5, 6), cpy, "copy ctor value");
    assertTrue(&cpy != &src, "copy ctor makes a distinct object");

    // A copy must be independent: mutating one must not touch the other.
    cpy[0] = 99;
    check(vect2(5, 6), src, "source unchanged after mutating the copy");
    check(vect2(99, 6), cpy, "copy holds the new value");

    vect2 init = src;                       // copy-initialisation
    check(vect2(5, 6), init, "copy-init  vect2 v = src");

    const vect2 c(src);                     // copy ctor from non-const into const
    check(vect2(5, 6), c, "copy ctor into const");

    vect2 fromConst(c);                     // and back out of a const source
    check(vect2(5, 6), fromConst, "copy ctor from const source");
}

static void testAssignment() {
    section("Assignment");

    vect2 a(56, 32);
    vect2 b;
    vect2& ret = (b = a);
    check(vect2(56, 32), b, "b = a");
    assertTrue(&ret == &b, "operator= returns *this (not a copy)");
    check(vect2(56, 32), a, "right-hand side unchanged");

    vect2 x, y, z(5, 6);
    x = y = z;
    check(vect2(5, 6), x, "chained  x = y = z  (x)");
    check(vect2(5, 6), y, "chained  x = y = z  (y)");

    // Self-assignment, both spellings. The second one is what an identity
    // guard (this != &other) actually protects.
    vect2 s(7, 9);
    s = s;
    check(vect2(7, 9), s, "self-assignment  s = s");

    vect2& alias = s;
    s = alias;
    check(vect2(7, 9), s, "self-assignment through an alias");

    // Assigning equal values must still be a well-defined no-op. This catches
    // a value-based guard (if (*this != other)) doing the wrong thing.
    vect2 p(1, 2), q(1, 2);
    p = q;
    check(vect2(1, 2), p, "assignment of an equal value");

    // Overwriting with a smaller/negative value must not be skipped.
    vect2 big(1000, 1000);
    big = vect2(-1, -2);
    check(vect2(-1, -2), big, "assignment overwrites with negatives");

    // (a = b) is an lvalue, so it can be assigned again.
    vect2 t1(0, 0), t2(3, 3), t3(4, 4);
    (t1 = t2) = t3;
    check(vect2(4, 4), t1, "(t1 = t2) = t3");
}

static void testSubscript() {
    section("operator[]");

    vect2 v(4, 9);
    check(4, v[0], "non-const read [0]");
    check(9, v[1], "non-const read [1]");

    const vect2 c(4, 9);
    check(4, c[0], "const read [0]");
    check(9, c[1], "const read [1]");

    v[0] = 10;
    check(vect2(10, 9), v, "write [0] leaves y alone");
    v[1] = 20;
    check(vect2(10, 20), v, "write [1] leaves x alone");

    // The non-const overload hands back a real reference into the object.
    int& rx = v[0];
    rx = -7;
    check(-7, v[0], "int& from [0] aliases the member");

    int& ry = v[1];
    ry = -8;
    check(-8, v[1], "int& from [1] aliases the member");

    // Writes through [] must be visible to every other operation.
    vect2 w;
    w[0] = 3;
    w[1] = 4;
    check(vect2(6, 8), w * 2, "value written via [] feeds operator*");
    check("{3, 4}", streamed(w), "value written via [] feeds operator<<");

    // Compound assignment straight through the reference.
    vect2 z(1, 1);
    z[0] += 5;
    z[1] *= 3;
    check(vect2(6, 3), z, "z[0] += 5 ; z[1] *= 3");
}

static void testStream() {
    section("operator<<");

    checkFormat(vect2(), "format: default");
    checkFormat(vect2(1, 2), "format: positive");
    checkFormat(vect2(-3, -4), "format: negative");
    checkFormat(vect2(0, -1), "format: zero and negative");
    checkFormat(vect2(INT_MAX, INT_MIN), "format: extremes");

    check("{1, 2}", streamed(vect2(1, 2)), "exact text  {1, 2}");
    check("{0, 0}", streamed(vect2()), "exact text  {0, 0}");
    check("{-84, -168}", streamed(vect2(-84, -168)), "exact text  {-84, -168}");

    // Must return the stream so it chains, and must not touch the stream state.
    std::ostringstream os;
    std::ostream& back = (os << vect2(1, 2));
    assertTrue(&back == &os, "operator<< returns the same stream");
    assertTrue(os.good(), "operator<< leaves the stream in a good state");

    os << vect2(3, 4) << "|" << vect2(5, 6);
    check("{1, 2}{3, 4}|{5, 6}", os.str(), "chained <<");

    // Printing a const vect2 and a temporary must both work.
    const vect2 c(7, 8);
    check("{7, 8}", streamed(c), "streaming a const vect2");
    check("{9, 9}", streamed(vect2(9, 9)), "streaming a temporary");
}

static void testArithmetic() {
    section("Arithmetic  +  -  unary-  *scalar");

    vect2 a(56, 32), b(1, 2);
    check(vect2(57, 34), a + b, "a + b");
    check(vect2(55, 30), a - b, "a - b");
    check(vect2(-55, -30), b - a, "b - a (not commutative)");

    // Non-mutating: the operands must survive untouched.
    check(vect2(56, 32), a, "a unchanged after a+b / a-b");
    check(vect2(1, 2), b, "b unchanged after a+b / a-b");

    check(vect2(-3, 4), -vect2(3, -4), "unary -v");
    check(vect2(3, -4), -(-vect2(3, -4)), "double negation");
    check(vect2(0, 0), -vect2(0, 0), "-(0,0) == (0,0)");

    vect2 n(3, -4);
    (void)-n;
    check(vect2(3, -4), n, "unary - does not mutate its operand");

    check(vect2(6, 8), vect2(3, 4) * 2, "v * 2");
    check(vect2(9, 12), 3 * vect2(3, 4), "3 * v  (free operator)");
    check(vect2(3, 4) * 7, 7 * vect2(3, 4), "v * n == n * v");
    check(vect2(0, 0), vect2(3, 4) * 0, "v * 0");
    check(vect2(3, 4), vect2(3, 4) * 1, "v * 1");
    check(vect2(-3, -4), vect2(3, 4) * -1, "v * -1");
    check(-vect2(3, 4), vect2(3, 4) * -1, "v * -1 == -v");

    // Algebraic identities.
    vect2 u(11, -13), w(-5, 7);
    check(u + w, w + u, "commutativity of +");
    check(vect2(0, 0), u - u, "u - u == 0");
    check(u, u + vect2(0, 0), "u + 0 == u");
    check(u - w, u + (-w), "u - w == u + (-w)");
    check((u + w) * 3, u * 3 + w * 3, "distributivity of scalar *");
    check(u * 6, (u * 2) * 3, "(u*2)*3 == u*6");

    // Deep nesting / mixed chains.
    check(vect2(1, 2), vect2(1, 2) - vect2(1, 2) + vect2(1, 2), "v - v + v");
    check(vect2(10, 20), 2 * (vect2(1, 2) + vect2(4, 8)) - vect2(0, 0), "2*(a+b) - 0");
    check(vect2(-6, -6), -(vect2(1, 1) + vect2(2, 2)) * 2, "-(a+b)*2");
}

static void testConstOperands() {
    section("const operands");

    // Everything the subject uses on a const vect2 must be a const member.
    const vect2 c(1, 2);
    const vect2 d(3, 4);

    check(vect2(4, 6), c + d, "const + const");
    check(vect2(-2, -2), c - d, "const - const");
    check(vect2(2, 4), c * 2, "const * scalar");
    check(vect2(2, 4), 2 * c, "scalar * const");
    check(vect2(-1, -2), -c, "-const");
    check(2, c[1], "const [1]");
    assertTrue(c == vect2(1, 2), "const ==");
    assertTrue(c != d, "const !=");
    check(vect2(3, 6), c + c * 2, "subject: v3 + v3 * 2 on a const");

    // Mixing const and non-const operands, both ways round.
    vect2 m(10, 10);
    check(vect2(11, 12), m + c, "non-const + const");
    check(vect2(11, 12), c + m, "const + non-const");
    check(vect2(9, 8), m - c, "non-const - const");
    m += c;
    check(vect2(11, 12), m, "non-const += const");
}

static void testIncrementDecrement() {
    section("Increment / decrement");

    vect2 v(1, 2);
    check(vect2(1, 2), v++, "v++ yields the OLD value");
    check(vect2(2, 3), v, "state after v++");
    check(vect2(3, 4), ++v, "++v yields the NEW value");
    check(vect2(3, 4), v, "state after ++v");
    check(vect2(3, 4), v--, "v-- yields the OLD value");
    check(vect2(2, 3), v, "state after v--");
    check(vect2(1, 2), --v, "--v yields the NEW value");
    check(vect2(1, 2), v, "state after --v");

    // Prefix returns a reference to the object itself.
    vect2 p(0, 0);
    vect2& pref = ++p;
    assertTrue(&pref == &p, "++v returns *this");
    vect2& mref = --p;
    assertTrue(&mref == &p, "--v returns *this");

    // ...so it chains, and each chained call must actually apply.
    vect2 w(0, 0);
    ++(++w);
    check(vect2(2, 2), w, "++(++w)");
    --(--w);
    check(vect2(0, 0), w, "--(--w)");

    // Postfix returns a *copy*: incrementing that copy must not reach w.
    vect2 q(0, 0);
    (q++)++;
    check(vect2(1, 1), q, "(q++)++ increments q only once");

    // Round trips.
    vect2 r(41, 41);
    r++; r--;
    check(vect2(41, 41), r, "++ then -- is a round trip");
    ++r; --r;
    check(vect2(41, 41), r, "prefix ++ then -- is a round trip");

    // Both components move together, from asymmetric values.
    vect2 asym(-1, 100);
    ++asym;
    check(vect2(0, 101), asym, "++ steps both components");
    --asym; --asym;
    check(vect2(-2, 99), asym, "-- steps both components");

    // Crossing zero.
    vect2 zero(0, 0);
    --zero;
    check(vect2(-1, -1), zero, "-- through zero");
    ++zero; ++zero;
    check(vect2(1, 1), zero, "++ back through zero");
}

static void testCompound() {
    section("Compound assignment");

    vect2 v(10, 20);
    vect2& r1 = (v += vect2(1, 2));
    check(vect2(11, 22), v, "v += (1,2)");
    assertTrue(&r1 == &v, "operator+= returns *this");

    vect2& r2 = (v -= vect2(1, 2));
    check(vect2(10, 20), v, "v -= (1,2)");
    assertTrue(&r2 == &v, "operator-= returns *this");

    vect2& r3 = (v *= 3);
    check(vect2(30, 60), v, "v *= 3");
    assertTrue(&r3 == &v, "operator*= returns *this");

    // The right-hand operand must not be modified.
    vect2 lhs(1, 1), rhs(2, 3);
    lhs += rhs;
    check(vect2(2, 3), rhs, "+= leaves the rhs alone");
    lhs -= rhs;
    check(vect2(2, 3), rhs, "-= leaves the rhs alone");

    check(vect2(0, 0), (vect2(5, 5) *= 0), "*= 0");
    check(vect2(5, 5), (vect2(5, 5) *= 1), "*= 1");
    check(vect2(-5, -5), (vect2(5, 5) *= -1), "*= -1");

    // Self-referencing compound assignment (aliased operands).
    vect2 s(3, 4);
    s += s;
    check(vect2(6, 8), s, "s += s  (doubling)");
    s -= s;
    check(vect2(0, 0), s, "s -= s  (zeroing)");

    // Nested: the inner call runs first, then the outer sees the new value on
    // both sides. This is the subject's  v2 += v2 += v3  shape.
    vect2 a(2, 3), b(1, 2);
    a += a += b;
    check(vect2(6, 10), a, "a += a += b");

    vect2 a2(10, 10), b2(1, 1);
    a2 -= a2 -= b2;
    check(vect2(0, 0), a2, "a -= a -= b");

    // Chaining off the returned reference.
    vect2 c(0, 0);
    (c += vect2(1, 1)) += vect2(2, 2);
    check(vect2(3, 3), c, "(c += ) += ");
    (c *= 2) += vect2(1, 1);
    check(vect2(7, 7), c, "(c *= 2) += ");

    // Compound must agree with the equivalent binary form.
    vect2 x(7, -3), y(-2, 5);
    vect2 viaCompound(x);
    viaCompound += y;
    check(x + y, viaCompound, "x += y  matches  x + y");
    viaCompound = x;
    viaCompound -= y;
    check(x - y, viaCompound, "x -= y  matches  x - y");
    viaCompound = x;
    viaCompound *= 4;
    check(x * 4, viaCompound, "x *= 4  matches  x * 4");
}

static void testComparison() {
    section("Comparison  ==  !=");

    vect2 a(1, 2), b(1, 2), cx(1, 3), cy(2, 2), dd(2, 3);

    check(true,  a == b,  "equal");
    check(false, a == cx, "y differs");
    check(false, a == cy, "x differs");
    check(false, a == dd, "both differ");
    check(true,  a == a,  "reflexive ==");

    check(false, a != b,  "equal !=");
    check(true,  a != cx, "y differs !=");
    check(true,  a != cy, "x differs !=");
    check(false, a != a,  "reflexive !=");

    // == and != must be exact negations of one another, and symmetric.
    check(a == b, b == a, "symmetry of ==");
    check(a != cx, cx != a, "symmetry of !=");
    check(!(a == cx), a != cx, "!= is the negation of ==");
    check(!(a == a), a != a, "!= is the negation of == (self)");

    // (1,2) vs (2,1): a sloppy implementation comparing sums would say equal.
    check(false, vect2(1, 2) == vect2(2, 1), "(1,2) != (2,1)");
    check(false, vect2(0, 5) == vect2(5, 0), "(0,5) != (5,0)");
    check(false, vect2(-1, 1) == vect2(1, -1), "(-1,1) != (1,-1)");

    // Comparing against temporaries and const objects.
    const vect2 k(4, 5);
    check(true, k == vect2(4, 5), "const == temporary");
    check(true, vect2(4, 5) == k, "temporary == const");
    check(true, vect2(0, 0) == vect2(), "temporary == default");
}

static void testBoundaries() {
    section("Boundary values (well-defined int cases)");

    vect2 hi(INT_MAX, INT_MAX);
    vect2 lo(INT_MIN, INT_MIN);

    check(INT_MAX, hi[0], "INT_MAX survives construction");
    check(INT_MIN, lo[1], "INT_MIN survives construction");
    check(vect2(INT_MAX, INT_MIN), vect2(INT_MAX, INT_MIN), "mixed extremes compare equal");
    assertTrue(hi != lo, "INT_MAX vect2 != INT_MIN vect2");

    check(vect2(0, 0), hi - hi, "INT_MAX - INT_MAX");
    check(vect2(0, 0), lo - lo, "INT_MIN - INT_MIN");
    check(vect2(-1, -1), hi + lo, "INT_MAX + INT_MIN == -1");
    check(vect2(-INT_MAX, -INT_MAX), -hi, "-INT_MAX");
    check(hi, hi * 1, "INT_MAX * 1");
    check(lo, lo * 1, "INT_MIN * 1");
    check(vect2(0, 0), hi * 0, "INT_MAX * 0");
    check(vect2(0, 0), lo * 0, "INT_MIN * 0");

    vect2 nearTop(INT_MAX - 1, INT_MAX - 1);
    ++nearTop;
    check(hi, nearTop, "++ up to INT_MAX");
    --nearTop;
    check(vect2(INT_MAX - 1, INT_MAX - 1), nearTop, "-- back down from INT_MAX");

    vect2 nearBottom(INT_MIN + 1, INT_MIN + 1);
    --nearBottom;
    check(lo, nearBottom, "-- down to INT_MIN");
    ++nearBottom;
    check(vect2(INT_MIN + 1, INT_MIN + 1), nearBottom, "++ back up from INT_MIN");

    check("{2147483647, -2147483648}", streamed(vect2(INT_MAX, INT_MIN)), "extremes print in full");
}

// The exact program from the subject, with every intermediate state asserted.
static void testSubjectTrace() {
    section("Subject main, step by step");

    vect2       v1;
    vect2       v2(1, 2);
    const vect2 v3(v2);
    vect2       v4 = v2;

    check(vect2(0, 0), v1, "v1 default");
    check(vect2(1, 2), v2, "v2(1,2)");
    check(vect2(1, 2), v3, "v3 copy of v2 (const)");
    check(vect2(1, 2), v4, "v4 = v2");
    check(canonical(v1), streamed(v1), "v1 prints canonically");

    check(vect2(1, 2), v4++, "v4++ prints the old value");
    check(vect2(2, 3), v4, "  -> v4 is now 2, 3");
    check(vect2(3, 4), ++v4, "++v4 prints the new value");
    check(vect2(3, 4), v4, "  -> v4 is now 3, 4");
    check(vect2(3, 4), v4--, "v4-- prints the old value");
    check(vect2(2, 3), v4, "  -> v4 is now 2, 3");
    check(vect2(1, 2), --v4, "--v4 prints the new value");
    check(vect2(1, 2), v4, "  -> v4 is now 1, 2");

    v2 += v3;           check(vect2(2, 4),      v2, "v2 += v3");
    v1 -= v2;           check(vect2(-2, -4),    v1, "v1 -= v2");
    v2 = v3 + v3 * 2;   check(vect2(3, 6),      v2, "v2 = v3 + v3 * 2");
    v2 = 3 * v2;        check(vect2(9, 18),     v2, "v2 = 3 * v2");
    v2 += v2 += v3;     check(vect2(20, 40),    v2, "v2 += v2 += v3");
    v1 *= 42;           check(vect2(-84, -168), v1, "v1 *= 42");
    v1 = v1 - v1 + v1;  check(vect2(-84, -168), v1, "v1 = v1 - v1 + v1");

    check(vect2(1, 2), v3, "v3 (const) never changed");

    check("{-84, -168}", streamed(v1), "v1 output");
    check("{20, 40}", streamed(v2), "v2 output");
    check("{-20, -40}", streamed(-v2), "-v2 output");
    check(-168, v1[1], "v1[1] before write");
    v1[1] = 12;
    check(12, v1[1], "v1[1] after write");
    check(-84, v1[0], "v1[0] untouched by the write");
    check(2, v3[1], "v3[1] (const read)");

    check(false, v1 == v3, "v1 == v3");
    check(true,  v1 == v1, "v1 == v1");
    check(true,  v1 != v3, "v1 != v3");
    check(false, v1 != v1, "v1 != v1");
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "-v")
        vtest::verbose() = true;

    testConstruction();
    testCopy();
    testAssignment();
    testSubscript();
    testStream();
    testArithmetic();
    testConstOperands();
    testIncrementDecrement();
    testCompound();
    testComparison();
    testBoundaries();
    testSubjectTrace();

    return (vtest::summary("unit checks"));
}
