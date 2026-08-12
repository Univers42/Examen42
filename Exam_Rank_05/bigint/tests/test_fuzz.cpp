#include "../bigint.hpp"
#include "vtest.hpp"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Differential fuzzing.
//
// Every bigint in play is shadowed by a `Model` that holds the same number as
// little-endian digits in a std::vector<int>. That is deliberately the mirror
// image of bigint's own representation (big-endian, in a std::string, mutated
// in place): a model written the same way round would share bigint's bugs and
// agree with it for the wrong reason.
//
// Sizes are capped. `x <<= k` allocates k digits, so an unbounded shift amount
// would ask for gigabytes rather than find a bug -- the fuzzer keeps left
// shifts small enough to stay a test instead of a memory benchmark. Right
// shifts have no such cost and are fuzzed with arbitrarily large amounts.
// ---------------------------------------------------------------------------

namespace {

const int    NOBJ      = 6;
const size_t MAXDIGITS = 400;   // ceiling on any model value's width
const size_t MAXSHL    = 60;    // ceiling on a single left shift

typedef std::vector<int> Num;   // little-endian digits; empty == 0

// --- reference arithmetic --------------------------------------------------

void trim(Num& n) {
    while (!n.empty() && n.back() == 0)
        n.erase(n.end() - 1);
}

std::string render(const Num& n) {
    if (n.empty())
        return ("0");
    std::string out;
    for (size_t i = n.size(); i-- > 0;)
        out += char('0' + n[i]);
    return (out);
}

Num fromInt(int v) {
    Num n;
    if (v < 0)
        v = 0;                  // bigint(int) clamps negatives to zero
    while (v > 0) {
        n.push_back(v % 10);
        v /= 10;
    }
    return (n);
}

Num fromText(const std::string& s) {
    Num n;
    for (size_t i = s.size(); i-- > 0;)
        n.push_back(s[i] - '0');
    trim(n);
    return (n);
}

Num add(const Num& a, const Num& b) {
    Num out;
    int carry = 0;
    size_t w = a.size() > b.size() ? a.size() : b.size();
    for (size_t i = 0; i < w; ++i) {
        int d = carry;
        if (i < a.size()) d += a[i];
        if (i < b.size()) d += b[i];
        out.push_back(d % 10);
        carry = d / 10;
    }
    if (carry)
        out.push_back(carry);
    trim(out);
    return (out);
}

Num shl(const Num& a, size_t k) {
    if (a.empty())
        return (a);             // 0 stays 0 however far it is shifted
    Num out(k, 0);
    out.insert(out.end(), a.begin(), a.end());
    return (out);
}

Num shr(const Num& a, size_t k) {
    if (k >= a.size())
        return (Num());
    Num out(a.begin() + long(k), a.end());
    trim(out);
    return (out);
}

// -1 / 0 / 1
int cmp(const Num& a, const Num& b) {
    if (a.size() != b.size())
        return (a.size() < b.size() ? -1 : 1);
    for (size_t i = a.size(); i-- > 0;)
        if (a[i] != b[i])
            return (a[i] < b[i] ? -1 : 1);
    return (0);
}

// The value as a shift amount, saturated: anything this wide is treated as
// "far larger than any value we hold".
size_t asShift(const Num& n) {
    if (n.size() > 6)
        return (~size_t(0));
    size_t k = 0;
    for (size_t i = n.size(); i-- > 0;)
        k = k * 10 + size_t(n[i]);
    return (k);
}

// --- random ----------------------------------------------------------------

// std::rand() is only guaranteed to reach 32767, so stack a few draws.
unsigned long rawRandom() {
    unsigned long r = 0;
    for (int i = 0; i < 3; ++i)
        r = (r << 11) ^ (unsigned long)std::rand();
    return (r);
}

size_t rndBelow(size_t n) { return ((size_t)(rawRandom() % n)); }
int    rndIndex(int n)    { return ((int)(rawRandom() % (unsigned long)n)); }

// Widths cluster at the boundaries that matter: one digit, the width where a
// carry has to cross into grafted digits, and the cap.
size_t rndWidth() {
    int bucket = rndIndex(100);
    if (bucket < 35) return (1 + rndBelow(3));
    if (bucket < 70) return (1 + rndBelow(20));
    if (bucket < 92) return (1 + rndBelow(MAXDIGITS / 2));
    return (1 + rndBelow(MAXDIGITS));
}

// Digit strings weighted towards the shapes that break carry handling: all
// nines, all zeros but one, a nine-run in the middle.
std::string rndDigits() {
    size_t w = rndWidth();
    int shape = rndIndex(100);
    std::string s(w, '0');
    if (shape < 55) {
        for (size_t i = 0; i < w; ++i)
            s[i] = char('0' + rndBelow(10));
    } else if (shape < 70) {
        s.assign(w, '9');
    } else if (shape < 80) {
        s.assign(w, '9');
        s[rndBelow(w)] = char('0' + rndBelow(9));
    } else if (shape < 88) {
        s.assign(w, '0');
        s[rndBelow(w)] = char('1' + rndBelow(9));
    } else if (shape < 95) {
        for (size_t i = 0; i < w; ++i)
            s[i] = char('0' + rndBelow(10));
        size_t run = rndBelow(w) + 1;
        for (size_t i = w - run; i < w; ++i)
            s[i] = '9';
    } else {
        s.assign(w, char('0' + rndBelow(10)));
    }
    return (s);
}

int rndInt() {
    int bucket = rndIndex(100);
    if (bucket < 40) return ((int)rndBelow(10));
    if (bucket < 70) return ((int)rndBelow(100000));
    if (bucket < 90) return ((int)rndBelow(2000000000u));
    static const int extreme[] = { 0, 1, 9, 10, 99, 100, INT_MAX, INT_MAX - 1, -1, INT_MIN };
    return (extreme[rndIndex((int)(sizeof(extreme) / sizeof(extreme[0])))]);
}

// --- the campaign ----------------------------------------------------------

std::string streamed(const bigint& b) {
    std::ostringstream os;
    os << b;
    return (os.str());
}

struct Fuzzer {
    bigint v[NOBJ];
    Num    m[NOBJ];
    long   applied;
    long   skipped;
    int    failures;

    Fuzzer() : applied(0), skipped(0), failures(0) {}

    void fail(const std::string& what, const std::string& want, const std::string& got) {
        if (failures++ < 12)
            std::cout << vtest::red() << "  [KO] " << vtest::off() << what
                      << "\n        expected: " << want << "\n        got:      " << got << "\n";
    }

    // The whole state is read back after every single step, so a corruption
    // is reported against the operation that caused it and not 900 steps later.
    void verifyAll(const std::string& step) {
        for (int i = 0; i < NOBJ; ++i) {
            std::string want = render(m[i]);
            std::string got  = streamed(v[i]);
            if (want != got) {
                std::ostringstream lbl;
                lbl << step << "  -> v[" << i << "]";
                fail(lbl.str(), want, got);
            }
        }
    }

    // Every relational operator over every ordered pair, against the model.
    void verifyComparisons() {
        for (int i = 0; i < NOBJ; ++i) {
            for (int j = 0; j < NOBJ; ++j) {
                int c = cmp(m[i], m[j]);
                bool bad = (v[i] <  v[j]) != (c <  0)
                        || (v[i] >  v[j]) != (c >  0)
                        || (v[i] <= v[j]) != (c <= 0)
                        || (v[i] >= v[j]) != (c >= 0)
                        || (v[i] == v[j]) != (c == 0)
                        || (v[i] != v[j]) != (c != 0);
                if (bad) {
                    std::ostringstream lbl;
                    lbl << "compare v[" << i << "]=" << render(m[i])
                        << " v[" << j << "]=" << render(m[j]);
                    std::ostringstream got;
                    got << (v[i] < v[j]) << (v[i] > v[j]) << (v[i] <= v[j])
                        << (v[i] >= v[j]) << (v[i] == v[j]) << (v[i] != v[j]);
                    std::ostringstream want;
                    want << (c < 0) << (c > 0) << (c <= 0) << (c >= 0) << (c == 0) << (c != 0);
                    fail(lbl.str(), want.str() + "  (< > <= >= == !=)", got.str());
                }
            }
        }
    }

    bool tooWide(const Num& n) { return (n.size() > MAXDIGITS); }

    void step() {
        // Operands are drawn independently on purpose, so a and b collide
        // often and every operation gets exercised on aliased arguments.
        int a = rndIndex(NOBJ), b = rndIndex(NOBJ), c = rndIndex(NOBJ);
        int op = rndIndex(17);
        std::ostringstream lbl;

        switch (op) {
        case 0: {
            lbl << "v[" << a << "] = v[" << b << "]";
            v[a] = v[b];
            m[a] = m[b];
            break;
        }
        case 1: {
            Num want = add(m[a], m[b]);
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] += v[" << b << "]";
            bigint& r = (v[a] += v[b]);
            if (&r != &v[a]) fail(lbl.str() + " (returned a copy)", "&v[a]", "another object");
            m[a] = want;
            break;
        }
        case 2: {
            Num want = add(m[b], m[c]);
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] = v[" << b << "] + v[" << c << "]";
            // The operands of a free operator+ must come out unchanged.
            Num keepB = m[b], keepC = m[c];
            v[a] = v[b] + v[c];
            m[a] = want;
            if (a != b && render(keepB) != streamed(v[b]))
                fail(lbl.str() + " (mutated its left operand)", render(keepB), streamed(v[b]));
            if (a != c && render(keepC) != streamed(v[c]))
                fail(lbl.str() + " (mutated its right operand)", render(keepC), streamed(v[c]));
            break;
        }
        case 3: {
            Num want = add(m[a], fromInt(1));
            if (tooWide(want)) { ++skipped; return; }
            lbl << "++v[" << a << "]";
            bigint& r = ++v[a];
            if (&r != &v[a]) fail(lbl.str() + " (returned a copy)", "&v[a]", "another object");
            m[a] = want;
            break;
        }
        case 4: {
            Num want = add(m[a], fromInt(1));
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "]++";
            std::string before = render(m[a]);
            bigint old = v[a]++;
            if (streamed(old) != before)
                fail(lbl.str() + " (postfix did not yield the old value)", before, streamed(old));
            m[a] = want;
            break;
        }
        case 5: {
            size_t k = rndBelow(MAXSHL + 1);
            Num want = shl(m[a], k);
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] <<= " << k;
            bigint& r = (v[a] <<= bigint((int)k));
            if (&r != &v[a]) fail(lbl.str() + " (returned a copy)", "&v[a]", "another object");
            m[a] = want;
            break;
        }
        case 6: {
            // Right shifts cost nothing, so the amount is allowed to be far
            // wider than the value it is applied to.
            size_t k = rndIndex(4) ? rndBelow(MAXDIGITS + 4) : rndBelow(1000000);
            lbl << "v[" << a << "] >>= " << k;
            bigint& r = (v[a] >>= bigint((int)k));
            if (&r != &v[a]) fail(lbl.str() + " (returned a copy)", "&v[a]", "another object");
            m[a] = shr(m[a], k);
            break;
        }
        case 7: {
            size_t k = rndBelow(MAXSHL + 1);
            Num want = shl(m[b], k);
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] = v[" << b << "] << " << k;
            Num keepB = m[b];
            v[a] = v[b] << bigint((int)k);
            m[a] = want;
            if (a != b && render(keepB) != streamed(v[b]))
                fail(lbl.str() + " (mutated its operand)", render(keepB), streamed(v[b]));
            break;
        }
        case 8: {
            size_t k = rndBelow(MAXDIGITS + 4);
            lbl << "v[" << a << "] = v[" << b << "] >> " << k;
            Num keepB = m[b];
            v[a] = v[b] >> bigint((int)k);
            m[a] = shr(m[b], k);
            if (a != b && render(keepB) != streamed(v[b]))
                fail(lbl.str() + " (mutated its operand)", render(keepB), streamed(v[b]));
            break;
        }
        case 9: {
            int n = rndInt();
            lbl << "v[" << a << "] = bigint(" << n << ")";
            v[a] = bigint(n);
            m[a] = fromInt(n);
            break;
        }
        case 10: {
            std::string t = rndDigits();
            lbl << "v[" << a << "] = bigint(\"" << t.substr(0, 24)
                << (t.size() > 24 ? "..." : "") << "\")";
            v[a] = bigint(t);
            m[a] = fromText(t);
            break;
        }
        case 11: {
            verifyComparisons();
            return;
        }
        case 12: {
            int n = rndInt();
            Num want = add(m[a], fromInt(n));
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] += " << n;
            v[a] += n;
            m[a] = want;
            break;
        }
        case 13: {
            int n = rndInt();
            Num want = add(m[b], fromInt(n));
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] = " << n << " + v[" << b << "]";
            v[a] = n + v[b];        // int on the left, converted implicitly
            m[a] = want;
            break;
        }
        case 14: {
            lbl << "v[" << a << "] = bigint(v[" << b << "])";
            bigint copy(v[b]);
            v[a] = copy;
            m[a] = m[b];
            break;
        }
        case 15: {
            // Shift amount held in another bigint. Only safe when that value
            // is small; a wide one would ask for a wide allocation.
            size_t k = asShift(m[b]);
            if (k > MAXSHL) { ++skipped; return; }
            Num want = shl(m[a], k);
            if (tooWide(want)) { ++skipped; return; }
            lbl << "v[" << a << "] <<= v[" << b << "]  (=" << k << ")";
            v[a] <<= v[b];
            m[a] = want;
            break;
        }
        case 16: {
            size_t k = asShift(m[b]);
            lbl << "v[" << a << "] >>= v[" << b << "]";
            v[a] >>= v[b];
            m[a] = shr(m[a], k);
            break;
        }
        }

        ++applied;
        verifyAll(lbl.str());
    }
};

// --- expression trees ------------------------------------------------------
//
// A flat list of operations never nests, so it never builds
// `((a + b) << 2) + (c >> 1)`. These do, evaluating the same random tree once
// with bigint and once with the model.

struct Tree {
    Num eval(bigint& out, int depth) {
        int kind = depth <= 0 ? 0 : rndIndex(10);
        if (kind < 3) {                               // leaf
            if (rndIndex(2)) {
                int n = rndInt();
                out = bigint(n);
                return (fromInt(n));
            }
            std::string t = rndDigits();
            out = bigint(t);
            return (fromText(t));
        }
        if (kind < 6) {                               // a + b
            bigint l, r;
            Num ml = eval(l, depth - 1);
            Num mr = eval(r, depth - 1);
            Num want = add(ml, mr);
            if (want.size() > MAXDIGITS) { out = l; return (ml); }
            out = l + r;
            return (want);
        }
        if (kind < 8) {                               // a << k
            bigint l;
            Num ml = eval(l, depth - 1);
            size_t k = rndBelow(MAXSHL + 1);
            Num want = shl(ml, k);
            if (want.size() > MAXDIGITS) { out = l; return (ml); }
            out = l << bigint((int)k);
            return (want);
        }
        if (kind < 9) {                               // a >> k
            bigint l;
            Num ml = eval(l, depth - 1);
            size_t k = rndBelow(MAXDIGITS + 4);
            out = l >> bigint((int)k);
            return (shr(ml, k));
        }
        // ++(a + b): a mutation applied to a temporary sub-expression
        bigint l, r;
        Num ml = eval(l, depth - 1);
        Num mr = eval(r, depth - 1);
        Num want = add(add(ml, mr), fromInt(1));
        if (want.size() > MAXDIGITS) { out = l; return (ml); }
        bigint t = l + r;
        out = ++t;
        return (want);
    }
};

void usage(const char* prog) {
    std::cout << "usage: " << prog
              << " [--seed N] [--steps N] [--exprs N] [--random]\n";
}

} // namespace

int main(int ac, char** av) {
    unsigned seed  = 1;
    long     steps = 200000;
    long     exprs = 20000;

    for (int i = 1; i < ac; ++i) {
        if (!std::strcmp(av[i], "--seed") && i + 1 < ac)
            seed = (unsigned)std::atoi(av[++i]);
        else if (!std::strcmp(av[i], "--steps") && i + 1 < ac)
            steps = std::atol(av[++i]);
        else if (!std::strcmp(av[i], "--exprs") && i + 1 < ac)
            exprs = std::atol(av[++i]);
        else if (!std::strcmp(av[i], "--random"))
            seed = (unsigned)std::time(0);
        else {
            usage(av[0]);
            return (1);
        }
    }

    std::srand(seed);
    std::cout << vtest::cyan() << "== stateful fuzz ==" << vtest::off()
              << "  seed " << seed << ", " << steps << " steps\n";

    Fuzzer f;
    for (long i = 0; i < steps && f.failures == 0; ++i)
        f.step();
    f.verifyComparisons();

    if (f.failures == 0)
        std::cout << vtest::green() << "  [ok] " << vtest::off()
                  << f.applied << " operations applied, " << f.skipped
                  << " skipped (would exceed the width cap)\n";

    std::cout << vtest::cyan() << "== expression trees ==" << vtest::off()
              << "  " << exprs << " trees\n";

    Tree t;
    long bad = 0;
    for (long i = 0; i < exprs && bad < 12; ++i) {
        bigint got;
        Num want = t.eval(got, 1 + rndIndex(4));
        std::ostringstream os;
        os << got;
        if (os.str() != render(want)) {
            ++bad;
            std::cout << vtest::red() << "  [KO] " << vtest::off() << "expression tree #" << i
                      << "\n        expected: " << render(want)
                      << "\n        got:      " << os.str() << "\n";
        }
    }
    if (bad == 0)
        std::cout << vtest::green() << "  [ok] " << vtest::off()
                  << exprs << " expression trees agree with the model\n";

    int failures = f.failures + (int)bad;
    std::cout << "\n-----------------------------\n";
    if (failures == 0) {
        std::cout << vtest::green() << "fuzzing found no difference from the model"
                  << vtest::off() << "\n";
        return (0);
    }
    std::cout << vtest::red() << failures << " difference(s) from the model"
              << vtest::off() << "\n";
    return (1);
}
