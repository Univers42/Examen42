#include "../bigint.hpp"
#include "vtest.hpp"
#include <climits>
#include <sstream>
#include <string>

using vtest::check;
using vtest::assertTrue;
using vtest::section;

// The only way to observe a bigint from the outside is operator<<, so every
// value check goes through it. That also means the format is under test all
// the time instead of only in its own section.
static std::string str(const bigint& b) {
    std::ostringstream os;
    os << b;
    return (os.str());
}

// "1" followed by n zeros == 10^n, and n nines == 10^n - 1. Every large
// expectation below is built from these two, so it can be checked by reading
// it rather than by trusting the implementation that produced it.
static std::string pow10(size_t n)  { return ("1" + std::string(n, '0')); }
static std::string nines(size_t n)  { return (std::string(n, '9')); }

// ---------------------------------------------------------------------------

static void testConstruction() {
    section("Construction");

    bigint d;
    check("0", str(d), "default ctor is 0");

    check("0", str(bigint(0)), "bigint(0)");
    check("7", str(bigint(7)), "bigint(7)");
    check("10", str(bigint(10)), "bigint(10)");
    check("42", str(bigint(42)), "bigint(42)");
    check("1337", str(bigint(1337)), "bigint(1337)");
    check("2147483647", str(bigint(INT_MAX)), "bigint(INT_MAX)");

    // The class stores an *unsigned* integer; a negative seed has no
    // representation, and clamping to 0 is what this implementation promises.
    check("0", str(bigint(-1)), "bigint(-1) clamps to 0");
    check("0", str(bigint(INT_MIN)), "bigint(INT_MIN) clamps to 0");

    // Every digit must survive, including the ones that are 0.
    check("1020304050", str(bigint(1020304050)), "interior zeros survive");

    section("Construction from a digit string");

    check("0", str(bigint(std::string("0"))), "string \"0\"");
    check("0", str(bigint(std::string(""))), "empty string is 0");
    check("0", str(bigint(std::string("0000"))), "all zeros is 0");
    check("42", str(bigint(std::string("42"))), "string \"42\"");
    check("42", str(bigint(std::string("0042"))), "leading zeros stripped");
    check("100", str(bigint(std::string("00100"))), "trailing zeros kept");
    check(pow10(500), str(bigint(pow10(500))), "500-digit value round-trips");
    check(nines(300), str(bigint(nines(300))), "300 nines round-trip");
    check("1", str(bigint(std::string(400, '0') + "1")), "400 leading zeros stripped");
}

static void testCopy() {
    section("Copy semantics");

    bigint src(1337);
    bigint cpy(src);
    check("1337", str(cpy), "copy ctor value");
    assertTrue(&cpy != &src, "copy ctor makes a distinct object");

    // Deep, not shared: mutating the copy must not reach the source.
    cpy += 1;
    check("1337", str(src), "source unchanged after mutating the copy");
    check("1338", str(cpy), "copy holds the new value");

    bigint init = src;                      // copy-initialisation
    check("1337", str(init), "copy-init  bigint v = src");

    const bigint c(src);
    check("1337", str(c), "copy ctor into const");

    bigint fromConst(c);
    check("1337", str(fromConst), "copy ctor from a const source");

    // A copy of a huge value must copy all of it, not a prefix.
    bigint huge(nines(1000));
    bigint hcopy(huge);
    check(nines(1000), str(hcopy), "copy of a 1000-digit value");
}

static void testAssignment() {
    section("Assignment");

    bigint a(56), b;
    bigint& ret = (b = a);
    check("56", str(b), "b = a");
    assertTrue(&ret == &b, "operator= returns *this (not a copy)");
    check("56", str(a), "right-hand side unchanged");

    bigint x, y, z(5);
    x = y = z;
    check("5", str(x), "chained  x = y = z  (x)");
    check("5", str(y), "chained  x = y = z  (y)");

    bigint s(79);
    s = s;
    check("79", str(s), "self-assignment  s = s");

    bigint& alias = s;
    s = alias;
    check("79", str(s), "self-assignment through an alias");

    // Overwriting a long value with a short one must shrink it, not leave a
    // tail of old digits behind.
    bigint big(nines(200));
    big = bigint(7);
    check("7", str(big), "long value overwritten by a short one");

    bigint small(1);
    small = bigint(nines(200));
    check(nines(200), str(small), "short value overwritten by a long one");

    // (a = b) is an lvalue, so it can be assigned again.
    bigint t1, t2(3), t3(4);
    (t1 = t2) = t3;
    check("4", str(t1), "(t1 = t2) = t3");
}

static void testStream() {
    section("Stream output");

    check("0", str(bigint()), "zero prints as a single 0");
    check("1", str(bigint(std::string("0001"))), "no leading zeros after construction");
    check("100", str(bigint(100)), "100 keeps its own zeros");

    // operator<< must return the stream so it can be chained, and must not
    // touch the value it printed.
    std::ostringstream os;
    bigint v(12);
    os << v << "-" << v;
    check("12-12", os.str(), "operator<< chains and does not consume");
    check("12", str(v), "value unchanged after printing");

    // A const bigint must be printable (the subject prints `const bigint a`).
    const bigint c(42);
    check("42", str(c), "const bigint is printable");

    // The width of the text is the number of digits, with no separators or
    // padding of any kind.
    check(size_t(1000), str(bigint(pow10(999))).size(), "10^999 prints 1000 chars");
}

static void testPlusEquals() {
    section("operator+=");

    bigint a(42);
    bigint& r = (a += bigint(21));
    check("63", str(a), "42 += 21");
    assertTrue(&r == &a, "operator+= returns *this (not a copy)");

    bigint z;
    z += bigint(0);
    check("0", str(z), "0 += 0");

    bigint id(1337);
    id += bigint(0);
    check("1337", str(id), "x += 0 leaves x alone");

    bigint zero;
    zero += bigint(1337);
    check("1337", str(zero), "0 += x becomes x");

    // The right-hand side must survive untouched.
    bigint lhs(5), rhs(7);
    lhs += rhs;
    check("7", str(rhs), "right-hand side unchanged by +=");

    section("operator+= carries");

    bigint c1(9);
    c1 += bigint(1);
    check("10", str(c1), "9 += 1 carries out");

    bigint c2(99);
    c2 += bigint(1);
    check("100", str(c2), "99 += 1 carries through");

    bigint c3(nines(50));
    c3 += bigint(1);
    check(pow10(50), str(c3), "50 nines += 1 == 10^50");

    bigint c4(nines(500));
    c4 += bigint(1);
    check(pow10(500), str(c4), "500 nines += 1 == 10^500");

    bigint c5(nines(100));
    c5 += bigint(nines(100));
    // (10^100 - 1) * 2 == 2*10^100 - 2 == "1" "9"x99 "8"
    check("1" + nines(99) + "8", str(c5), "100 nines doubled");

    section("operator+= with unequal widths");

    // The implementation grafts the wider operand's high digits in instead of
    // padding, so both directions and a carry into the grafted part matter.
    bigint w1(42);
    w1 += bigint(1337);
    check("1379", str(w1), "narrow += wide");

    bigint w2(1337);
    w2 += bigint(42);
    check("1379", str(w2), "wide += narrow");

    bigint w3(999);
    w3 += bigint(1001);
    check("2000", str(w3), "carry ripples into the grafted digits");

    bigint w4(999);
    w4 += bigint(std::string("1000000"));
    check("1000999", str(w4), "no carry into the grafted digits");

    bigint w5(1);
    w5 += bigint(nines(200));
    check(pow10(200), str(w5), "1 += 200 nines, carry across the whole graft");

    bigint w6(nines(200));
    w6 += bigint(1);
    check(pow10(200), str(w6), "200 nines += 1");

    bigint w7;
    w7 += bigint(pow10(300));
    check(pow10(300), str(w7), "0 += 10^300");

    section("operator+= aliasing");

    bigint s(21);
    s += s;
    check("42", str(s), "s += s doubles");

    bigint s2(nines(100));
    s2 += s2;
    check("1" + nines(99) + "8", str(s2), "s += s on 100 nines");

    bigint& sa = s;
    s += sa;
    check("84", str(s), "s += alias_of_s");

    bigint chain1(1), chain2(2), chain3(3);
    (chain1 += chain2) += chain3;
    check("6", str(chain1), "(a += b) += c");

    bigint n1(1), n2(2), n3(3);
    n1 += n2 += n3;
    check("6", str(n1), "a += b += c");
    check("5", str(n2), "  and b really became b + c");
}

static void testPlus() {
    section("operator+");

    check("63", str(bigint(42) + bigint(21)), "42 + 21");
    check("63", str(bigint(21) + bigint(42)), "21 + 42 (commutative)");
    check("0", str(bigint() + bigint()), "0 + 0");
    check("42", str(bigint(42) + bigint()), "x + 0");
    check("42", str(bigint() + bigint(42)), "0 + x");

    // Neither operand may be modified: operator+ takes its left side by value
    // precisely so that `a + b` cannot become `a += b`.
    bigint a(10), b(5);
    bigint sum = a + b;
    check("15", str(sum), "a + b");
    check("10", str(a), "a unchanged by a + b");
    check("5", str(b), "b unchanged by a + b");

    bigint self(7);
    check("14", str(self + self), "a + a");
    check("7", str(self), "a unchanged by a + a");

    // Implicit conversion from int, on either side.
    check("52", str(bigint(42) + 10), "bigint + int");
    check("52", str(10 + bigint(42)), "int + bigint");

    // Association and chaining.
    check("6", str(bigint(1) + bigint(2) + bigint(3)), "1 + 2 + 3");
    check(pow10(100), str(bigint(nines(100)) + 1), "10^100 - 1 + 1");
}

static void testIncrement() {
    section("Increment");

    bigint b(21);
    bigint& r = ++b;
    check("22", str(b), "++b increments");
    assertTrue(&r == &b, "prefix ++ returns *this (not a copy)");

    bigint p(21);
    bigint old = p++;
    check("21", str(old), "b++ yields the value from before");
    check("22", str(p), "b++ still increments");

    // If postfix returned *this, this would increment twice through the same
    // object and q would end at 12 instead of 11.
    bigint q(10);
    (q++)++;
    check("11", str(q), "(q++)++ increments q exactly once");

    bigint z;
    ++z;
    check("1", str(z), "++0 == 1");

    bigint nine(9);
    ++nine;
    check("10", str(nine), "++9 carries");

    bigint big(nines(100));
    ++big;
    check(pow10(100), str(big), "++ across 100 nines");

    // Repeated stepping over a carry boundary.
    bigint step(98);
    for (int i = 0; i < 4; ++i)
        ++step;
    check("102", str(step), "++ four times over 99/100");

    bigint pstep(98);
    for (int i = 0; i < 4; ++i)
        pstep++;
    check("102", str(pstep), "postfix ++ four times over 99/100");
}

static void testShiftLeft() {
    section("operator<<= and operator<<");

    bigint a(42);
    bigint& r = (a <<= bigint(3));
    check("42000", str(a), "42 <<= 3");
    assertTrue(&r == &a, "operator<<= returns *this (not a copy)");

    check("42", str(bigint(42) << bigint(0)), "x << 0 is x");
    check("42000", str(bigint(42) << bigint(3)), "42 << 3 == 42000 (subject)");
    check("1337000", str(bigint(1337) << 3), "int shift amount converts");

    // 0 must stay a single 0: appending zeros to it would produce "0000",
    // which is both wrong and un-normalised.
    check("0", str(bigint() << bigint(5)), "0 << 5 stays 0");
    bigint z;
    z <<= bigint(5);
    check("0", str(z), "0 <<= 5 stays 0");
    check(size_t(1), str(bigint() << bigint(100)).size(), "0 << 100 prints one char");

    // The operand of the free operator must survive.
    bigint src(7), k(2);
    bigint shifted = src << k;
    check("700", str(shifted), "src << k");
    check("7", str(src), "src unchanged by src << k");
    check("2", str(k), "k unchanged by src << k");

    check(pow10(300), str(bigint(1) << bigint(300)), "1 << 300 == 10^300");
    check("42" + std::string(200, '0'), str(bigint(42) << bigint(200)), "42 << 200");

    // Chained shifts add up.
    bigint c(1);
    c <<= bigint(10);
    c <<= bigint(10);
    check(pow10(20), str(c), "1 <<= 10 twice == 10^20");

    // Self-shift: the shift amount is read before the digits are appended.
    bigint s(3);
    s <<= s;
    check("3000", str(s), "s <<= s uses the old value as the amount");
}

static void testShiftRight() {
    section("operator>>= and operator>>");

    bigint a(1337);
    bigint& r = (a >>= bigint(2));
    check("13", str(a), "1337 >>= 2 == 13 (subject)");
    assertTrue(&r == &a, "operator>>= returns *this (not a copy)");

    check("1337", str(bigint(1337) >> bigint(0)), "x >> 0 is x");
    check("133", str(bigint(1337) >> bigint(1)), "1337 >> 1");
    check("1", str(bigint(1337) >> bigint(3)), "1337 >> 3");

    // Shifting away every digit gives 0, not an empty string.
    check("0", str(bigint(1337) >> bigint(4)), "1337 >> 4 == 0");
    check("0", str(bigint(1337) >> bigint(9)), "1337 >> 9 == 0");
    check("0", str(bigint() >> bigint(0)), "0 >> 0 == 0");
    check("0", str(bigint() >> bigint(5)), "0 >> 5 == 0");
    check(size_t(1), str(bigint(1337) >> bigint(4)).size(), "the result is one char, not empty");

    bigint z(9);
    z >>= bigint(1);
    check("0", str(z), "9 >>= 1 == 0");

    bigint src(700), k(2);
    bigint shifted = src >> k;
    check("7", str(shifted), "src >> k");
    check("700", str(src), "src unchanged by src >> k");
    check("2", str(k), "k unchanged by src >> k");

    // Truncation, not rounding.
    check("1", str(bigint(19) >> bigint(1)), "19 >> 1 truncates to 1");
    check("9", str(bigint(99) >> bigint(1)), "99 >> 1 truncates to 9");

    // Shifting right must not leave a leading zero behind.
    check("10", str(bigint(1050) >> bigint(2)), "1050 >> 2 == 10");
    check("1", str(bigint(1999) >> bigint(3)), "1999 >> 3 == 1");

    check("1", str(bigint(pow10(300)) >> bigint(300)), "10^300 >> 300 == 1");
    check(pow10(100), str(bigint(pow10(300)) >> bigint(200)), "10^300 >> 200 == 10^100");

    bigint s(2);
    s >>= s;
    check("0", str(s), "s >>= s");

    section("<< and >> are inverses (for non-zero values)");

    const char* vals[] = { "1", "42", "1337", "900", "1000000007" };
    for (int i = 0; i < 5; ++i) {
        for (int k2 = 0; k2 < 60; k2 += 7) {
            bigint v((std::string(vals[i])));
            bigint back = (v << bigint(k2)) >> bigint(k2);
            check(vals[i], str(back), std::string("(") + vals[i] + " << k) >> k");
        }
    }
}

// Every relational operator, checked against the answer a human would give.
static void checkOrder(const std::string& a, const std::string& b, int rel,
                       const std::string& label) {
    bigint x((a)), y((b));
    check(rel < 0, x < y,   label + "  <");
    check(rel > 0, x > y,   label + "  >");
    check(rel <= 0, x <= y, label + "  <=");
    check(rel >= 0, x >= y, label + "  >=");
    check(rel == 0, x == y, label + "  ==");
    check(rel != 0, x != y, label + "  !=");
}

static void testComparison() {
    section("Comparison");

    checkOrder("0", "0", 0, "0 vs 0");
    checkOrder("0", "1", -1, "0 vs 1");
    checkOrder("1", "0", 1, "1 vs 0");
    checkOrder("42", "42", 0, "42 vs 42");
    checkOrder("42", "1337", -1, "42 vs 1337");
    checkOrder("1337", "42", 1, "1337 vs 42");

    // Same width, so the tie-break has to be a real digit comparison.
    checkOrder("199", "200", -1, "199 vs 200 (same width)");
    checkOrder("200", "199", 1, "200 vs 199 (same width)");
    checkOrder("999", "111", 1, "999 vs 111 (same width)");
    checkOrder("100", "101", -1, "100 vs 101 (differ in the last digit)");
    checkOrder("101", "100", 1, "101 vs 100");

    // Different widths, where a plain string comparison gives the wrong answer.
    checkOrder("9", "10", -1, "9 vs 10 (lexicographic trap)");
    checkOrder("10", "9", 1, "10 vs 9");
    checkOrder("99999999", "100000000", -1, "99999999 vs 100000000");
    checkOrder("5", "1000000", -1, "5 vs 1000000");

    // Normalisation makes these equal despite different spellings.
    checkOrder("0", "0000", 0, "0 vs 0000");
    checkOrder("42", "0042", 0, "42 vs 0042");

    checkOrder(nines(200), pow10(200), -1, "200 nines vs 10^200");
    checkOrder(pow10(200), nines(200), 1, "10^200 vs 200 nines");
    checkOrder(pow10(500), pow10(500), 0, "10^500 vs itself");
    checkOrder(pow10(500) + "0", pow10(500), 1, "one extra digit wins");

    // Two long values that differ only in their very last digit.
    checkOrder(std::string(499, '7') + "3", std::string(499, '7') + "4", -1,
               "differ only in the last of 500 digits");

    section("Comparison consistency");

    // Exhaustive over a small window: every operator must agree with int.
    for (int i = 0; i < 24; ++i) {
        for (int j = 0; j < 24; ++j) {
            bigint x(i), y(j);
            std::ostringstream lbl;
            lbl << i << " vs " << j;
            check(i < j,  x < y,  lbl.str() + " <");
            check(i > j,  x > y,  lbl.str() + " >");
            check(i <= j, x <= y, lbl.str() + " <=");
            check(i >= j, x >= y, lbl.str() + " >=");
            check(i == j, x == y, lbl.str() + " ==");
            check(i != j, x != y, lbl.str() + " !=");
        }
    }

    // Self-comparison: a value is never less than itself and always equal.
    bigint self(1337);
    assertTrue(!(self < self), "!(x < x)");
    assertTrue(!(self > self), "!(x > x)");
    assertTrue(self <= self, "x <= x");
    assertTrue(self >= self, "x >= x");
    assertTrue(self == self, "x == x");
    assertTrue(!(self != self), "!(x != x)");

    // Comparison against an int operand, on either side.
    assertTrue(bigint(42) == 42, "bigint == int");
    assertTrue(42 == bigint(42), "int == bigint");
    assertTrue(bigint(41) < 42, "bigint < int");
    assertTrue(41 < bigint(42), "int < bigint");

    // A const bigint must be comparable (the subject compares `const bigint a`).
    const bigint c(42);
    assertTrue(c == bigint(42), "const bigint compares");
    assertTrue(!(c < c), "const bigint on both sides");
}

static void testSubjectExpressions() {
    section("Subject expressions");

    const bigint a(42);
    bigint b(21), c, d(1337), e(d);

    check("42", str(a), "a = 42");
    check("21", str(b), "b = 21");
    check("0", str(c), "c = 0");
    check("1337", str(d), "d = 1337");
    check("1337", str(e), "e = 1337 (copy of d)");

    check("63", str(a + b), "a + b = 63");
    check("42", str(c += a), "(c += a) = 42");
    check("42", str(c), "  c really is 42 afterwards");
    check("42", str(a), "  a unchanged (it is const)");

    check("21", str(b), "b = 21");
    check("22", str(++b), "++b = 22");
    check("22", str(b++), "b++ = 22 (old value)");
    check("23", str(b), "  b is 23 afterwards");

    check("230000000042", str((b << 10) + 42), "(b << 10) + 42");
    check("23", str(b), "  b unchanged by (b << 10)");

    check("13370000", str(d <<= 4), "(d <<= 4) = 13370000");
    check("133700", str(d >>= (const bigint)2), "(d >>= 2) = 133700");
    check("133700", str(d), "  d really is 133700");

    check(false, d < a, "(d < a) = 0");
    check(false, d <= a, "(d <= a) = 0");
    check(true, d > a, "(d > a) = 1");
    check(true, d >= a, "(d >= a) = 1");
    check(false, d == a, "(d == a) = 0");
    check(true, d != a, "(d != a) = 1");

    // The subject's example identities, spelled out.
    check("42000", str(bigint(42) << bigint(3)), "subject: 42 << 3 == 42000");
    check("13", str(bigint(1337) >> bigint(2)), "subject: 1337 >> 2 == 13");
}

static void testCombined() {
    section("Combined expressions");

    // A shift feeding an addition feeding a comparison, all on temporaries.
    assertTrue(((bigint(1) << bigint(3)) + 1) == bigint(1001), "(1 << 3) + 1 == 1001");
    assertTrue(((bigint(1234) >> bigint(2)) + 88) == bigint(100), "(1234 >> 2) + 88 == 100");

    // Left-to-right chaining without parentheses relies on << returning a new
    // bigint each time.
    check("1200", str(bigint(12) << bigint(1) << bigint(1)), "12 << 1 << 1");
    check("12", str(bigint(1200) >> bigint(1) >> bigint(1)), "1200 >> 1 >> 1");

    // The classic "accumulate" loop: 10 doublings of 1 is 1024.
    bigint acc(1);
    for (int i = 0; i < 10; ++i)
        acc += acc;
    check("1024", str(acc), "1 doubled ten times == 1024");

    // Fibonacci to 100, whose 100th term is a known 21-digit number.
    bigint f0, f1(1);
    for (int i = 0; i < 100; ++i) {
        bigint next = f0 + f1;
        f0 = f1;
        f1 = next;
    }
    check("354224848179261915075", str(f0), "fib(100) == 354224848179261915075");

    // 10^k built two ways must agree.
    for (int k = 0; k < 40; ++k) {
        bigint byShift = bigint(1) << bigint(k);
        bigint byText((pow10(size_t(k))));
        std::ostringstream lbl;
        lbl << "10^" << k << " by shift == by text";
        check(str(byText), str(byShift), lbl.str());
    }

    // Summing 1 a thousand times must land exactly on 1000.
    bigint count;
    for (int i = 0; i < 1000; ++i)
        ++count;
    check("1000", str(count), "1000 increments from 0");
}

int main(int ac, char** av) {
    if (ac > 1 && std::string(av[1]) == "-v")
        vtest::verbose() = true;

    testConstruction();
    testCopy();
    testAssignment();
    testStream();
    testPlusEquals();
    testPlus();
    testIncrement();
    testShiftLeft();
    testShiftRight();
    testComparison();
    testSubjectExpressions();
    testCombined();

    return (vtest::summary("checks"));
}
