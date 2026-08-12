#include "../searchable_array_bag.hpp"
#include "../searchable_tree_bag.hpp"
#include "../set.hpp"
#include "vtest.hpp"
#include <algorithm>
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
// Every bag in play is shadowed by a std::vector<int> holding the same values
// in insertion order. The expected print() output falls straight out of that
// vector: as written for an array bag, and sorted for a tree bag -- which is
// the whole observable difference between the two implementations.
//
// Three campaigns:
//   1. bags   -- insert / insert(array) / clear / copy / assign, with the
//                operands allowed to alias so self-assignment happens often.
//   2. sets   -- the same, through the set wrapper, whose contract is that a
//                value already in the wrapped bag is not inserted again.
//   3. views  -- several sets over one shared bag, plus writes made directly
//                to the bag behind their backs.
//
// Sizes are capped: tree_bag's destructor recurses, so an unbounded run of
// ascending inserts would overflow the stack rather than find a bug.
// ---------------------------------------------------------------------------

namespace {

const int    NOBJ    = 5;
const size_t MAXSIZE = 600;     // ceiling on how many values one bag holds

// --- random ----------------------------------------------------------------

// std::rand() is only guaranteed to reach 32767, so stack a few draws.
unsigned long rawRandom() {
    unsigned long r = 0;
    for (int i = 0; i < 3; ++i)
        r = (r << 11) ^ (unsigned long)std::rand();
    return (r);
}

int rndIndex(int n) { return ((int)(rawRandom() % (unsigned long)n)); }

// A narrow value range on purpose: duplicates and near-misses are where both
// has() implementations are most likely to disagree with the model.
int rndValue() {
    int bucket = rndIndex(100);
    if (bucket < 50) return (rndIndex(20) - 10);
    if (bucket < 80) return (rndIndex(400) - 200);
    if (bucket < 94) return ((int)(rawRandom() % 2000001UL) - 1000000);
    static const int extreme[] = { 0, 1, -1, INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1 };
    return (extreme[rndIndex((int)(sizeof(extreme) / sizeof(extreme[0])))]);
}

// --- the model -------------------------------------------------------------

typedef std::vector<int> Model;

std::string renderModel(const Model& m, bool sorted) {
    Model v(m);
    if (sorted)
        std::sort(v.begin(), v.end());
    std::ostringstream os;
    for (size_t i = 0; i < v.size(); ++i)
        os << v[i] << " ";
    os << "\n";
    return (os.str());
}

bool modelHas(const Model& m, int v) {
    return (std::find(m.begin(), m.end(), v) != m.end());
}

std::string printed(const bag& b) {
    std::ostringstream os;
    std::streambuf* old = std::cout.rdbuf(os.rdbuf());
    b.print();
    std::cout.rdbuf(old);
    return (os.str());
}

std::string printed(const set& s) {
    std::ostringstream os;
    std::streambuf* old = std::cout.rdbuf(os.rdbuf());
    s.print();
    std::cout.rdbuf(old);
    return (os.str());
}

// Shortened for the failure report: a 600-value line is unreadable.
std::string brief(const std::string& s) {
    if (s.size() <= 110)
        return (s);
    return (s.substr(0, 55) + " ... " + s.substr(s.size() - 50));
}

int failures = 0;

void fail(const std::string& what, const std::string& want, const std::string& got) {
    if (failures++ < 12)
        std::cout << vtest::red() << "  [KO] " << vtest::off() << what
                  << "\n        expected: " << brief(want)
                  << "\n        got:      " << brief(got) << "\n";
}

// --- campaign 1: the bags themselves ---------------------------------------

template <typename Bag>
struct BagFuzzer {
    Bag    v[NOBJ];
    Model  m[NOBJ];
    bool   sorted;
    std::string what;
    long   applied;
    long   skipped;

    BagFuzzer(bool s, const std::string& w)
        : sorted(s), what(w), applied(0), skipped(0) {}

    void verify(const std::string& step) {
        for (int i = 0; i < NOBJ; ++i) {
            std::string want = renderModel(m[i], sorted);
            std::string got  = printed(v[i]);
            if (want != got) {
                std::ostringstream lbl;
                lbl << what << ": " << step << "  -> v[" << i << "] print";
                fail(lbl.str(), want, got);
                return;
            }
        }
    }

    // has() is checked on values that are in the bag, values that are one
    // away from one that is, and values drawn at random.
    void verifyHas(const std::string& step) {
        for (int i = 0; i < NOBJ; ++i) {
            for (int probe = 0; probe < 6; ++probe) {
                int q;
                if (!m[i].empty() && probe < 4) {
                    q = m[i][(size_t)rndIndex((int)m[i].size())];
                    if (probe & 1)
                        q = (q == INT_MAX) ? q - 1 : q + 1;
                } else {
                    q = rndValue();
                }
                bool want = modelHas(m[i], q);
                bool got  = v[i].has(q);
                if (want != got) {
                    std::ostringstream lbl;
                    lbl << what << ": " << step << "  -> v[" << i << "].has(" << q << ")";
                    fail(lbl.str(), want ? "true" : "false", got ? "true" : "false");
                    return;
                }
            }
        }
    }

    void step() {
        // Drawn independently so a and b collide often: every copy and every
        // assignment gets exercised on aliased arguments.
        int a = rndIndex(NOBJ), b = rndIndex(NOBJ);
        int op = rndIndex(10);
        std::ostringstream lbl;

        switch (op) {
        case 0:
        case 1:
        case 2: {
            if (m[a].size() >= MAXSIZE) { ++skipped; return; }
            int x = rndValue();
            lbl << "v[" << a << "].insert(" << x << ")";
            v[a].insert(x);
            m[a].push_back(x);
            break;
        }
        case 3: {
            int n = rndIndex(12);
            if (m[a].size() + (size_t)n > MAXSIZE) { ++skipped; return; }
            std::vector<int> arr;
            for (int i = 0; i < n; ++i)
                arr.push_back(rndValue());
            lbl << "v[" << a << "].insert(arr, " << n << ")";
            std::vector<int> keep(arr);
            v[a].insert(n ? &arr[0] : (int*)0, n);
            if (arr != keep)
                fail(lbl.str() + " (modified the source array)", "unchanged", "modified");
            for (int i = 0; i < n; ++i)
                m[a].push_back(arr[(size_t)i]);
            break;
        }
        case 4: {
            lbl << "v[" << a << "].clear()";
            v[a].clear();
            m[a].clear();
            break;
        }
        case 5:
        case 6: {
            lbl << "v[" << a << "] = v[" << b << "]";
            Bag& r = (v[a] = v[b]);
            if (&r != &v[a])
                fail(lbl.str() + " (returned a copy)", "&v[a]", "another object");
            m[a] = m[b];
            break;
        }
        case 7: {
            lbl << "v[" << a << "] = Bag(v[" << b << "])";
            Bag copy(v[b]);
            // The copy must be independent: poke it, then check the source.
            std::string sourceBefore = printed(v[b]);
            if (m[b].size() < MAXSIZE)
                copy.insert(4242);
            if (printed(v[b]) != sourceBefore)
                fail(lbl.str() + " (copy shares state with its source)",
                     sourceBefore, printed(v[b]));
            v[a] = v[b];
            m[a] = m[b];
            break;
        }
        case 8: {
            // Assignment of a temporary copy, which exercises the destructor
            // of a live object in the same statement.
            lbl << "v[" << a << "] = Bag(v[" << b << "]) [temporary]";
            v[a] = Bag(v[b]);
            m[a] = m[b];
            break;
        }
        case 9: {
            // Through the abstract interfaces, so the virtual dispatch and the
            // shared virtual `bag` sub-object are on the hook too.
            if (m[a].size() >= MAXSIZE) { ++skipped; return; }
            int x = rndValue();
            lbl << "((searchable_bag&)v[" << a << "]).insert(" << x << ")";
            searchable_bag& s = v[a];
            bag&            g = v[a];
            s.insert(x);
            m[a].push_back(x);
            if (!s.has(x))
                fail(lbl.str() + " (has() through searchable_bag&)", "true", "false");
            // `bag` is a virtual base, so g and s must denote one object.
            if (printed(g) != renderModel(m[a], sorted))
                fail(lbl.str() + " (print() through bag&)",
                     renderModel(m[a], sorted), printed(g));
            break;
        }
        }

        ++applied;
        verify(lbl.str());
        verifyHas(lbl.str());
    }
};

// --- campaign 2: the set wrapper -------------------------------------------

// A set's model is the wrapped bag's model plus one rule: an insert whose
// value is already present is dropped.
struct SetFuzzer {
    searchable_array_bag arrayBags[NOBJ];
    searchable_tree_bag  treeBags[NOBJ];
    Model                m[2 * NOBJ];
    std::vector<set*>    sets;
    long                 applied;

    SetFuzzer() : applied(0) {
        for (int i = 0; i < NOBJ; ++i)
            sets.push_back(new set(arrayBags[i]));
        for (int i = 0; i < NOBJ; ++i)
            sets.push_back(new set(treeBags[i]));
    }

    ~SetFuzzer() {
        for (size_t i = 0; i < sets.size(); ++i)
            delete sets[i];
    }

    bool sortedAt(int i) { return (i >= NOBJ); }

    void verify(const std::string& step) {
        for (int i = 0; i < 2 * NOBJ; ++i) {
            std::string want = renderModel(m[i], sortedAt(i));
            std::string got  = printed(*sets[(size_t)i]);
            if (want != got) {
                std::ostringstream lbl;
                lbl << "set: " << step << "  -> s[" << i << "] print";
                fail(lbl.str(), want, got);
                return;
            }
            if (printed(sets[(size_t)i]->get_bag()) != got) {
                std::ostringstream lbl;
                lbl << "set: " << step << "  -> s[" << i << "].get_bag() print";
                fail(lbl.str(), got, printed(sets[(size_t)i]->get_bag()));
                return;
            }
        }
    }

    void step() {
        int a = rndIndex(2 * NOBJ);
        int op = rndIndex(8);
        std::ostringstream lbl;

        switch (op) {
        case 0:
        case 1:
        case 2:
        case 3: {
            if (m[a].size() >= MAXSIZE) return;
            int x = rndValue();
            lbl << "s[" << a << "].insert(" << x << ")";
            sets[(size_t)a]->insert(x);
            if (!modelHas(m[a], x))
                m[a].push_back(x);
            // The defining property: never a second copy of the same value.
            if (!sets[(size_t)a]->has(x))
                fail(lbl.str() + " (has() is false right after insert)", "true", "false");
            break;
        }
        case 4: {
            int n = rndIndex(10);
            if (m[a].size() + (size_t)n > MAXSIZE) return;
            std::vector<int> arr;
            for (int i = 0; i < n; ++i)
                arr.push_back(rndValue());
            lbl << "s[" << a << "].insert(arr, " << n << ")";
            sets[(size_t)a]->insert(n ? &arr[0] : (int*)0, n);
            for (int i = 0; i < n; ++i)
                if (!modelHas(m[a], arr[(size_t)i]))
                    m[a].push_back(arr[(size_t)i]);
            break;
        }
        case 5: {
            lbl << "s[" << a << "].clear()";
            sets[(size_t)a]->clear();
            m[a].clear();
            break;
        }
        case 6: {
            // A copy of a set is another view of the same bag, so an insert
            // through the copy must be visible through the original.
            lbl << "set copy of s[" << a << "]";
            set copy(*sets[(size_t)a]);
            if (&copy.get_bag() != &sets[(size_t)a]->get_bag())
                fail(lbl.str() + " (copy wraps a different bag)", "the same bag", "another bag");
            if (m[a].size() < MAXSIZE) {
                int x = rndValue();
                copy.insert(x);
                if (!modelHas(m[a], x))
                    m[a].push_back(x);
            }
            break;
        }
        case 7: {
            // Rebinding one view onto another set's bag.
            int b = rndIndex(2 * NOBJ);
            lbl << "s[" << a << "] = s[" << b << "]";
            set tmp(*sets[(size_t)a]);
            tmp = *sets[(size_t)b];
            if (&tmp.get_bag() != &sets[(size_t)b]->get_bag())
                fail(lbl.str() + " (operator= did not rebind)", "s[b]'s bag", "another bag");
            if (printed(tmp) != renderModel(m[b], sortedAt(b)))
                fail(lbl.str() + " (rebound view prints the wrong bag)",
                     renderModel(m[b], sortedAt(b)), printed(tmp));
            break;
        }
        }

        ++applied;
        verify(lbl.str());
    }
};

// --- campaign 3: several views over one bag --------------------------------

void sharedViews(long rounds) {
    searchable_array_bag ab;
    searchable_tree_bag  tb;
    set a1(ab), a2(ab), a3(ab);
    set t1(tb), t2(tb);
    Model ma, mt;

    set* av[3] = { &a1, &a2, &a3 };
    set* tv[2] = { &t1, &t2 };

    for (long i = 0; i < rounds && failures == 0; ++i) {
        int x = rndValue();
        std::ostringstream lbl;

        if (rndIndex(4)) {
            // Insert through a randomly chosen view. Whichever view is used,
            // the dedup rule is "is it already in the shared bag".
            lbl << "insert " << x << " through a view";
            av[rndIndex(3)]->insert(x);
            tv[rndIndex(2)]->insert(x);
            if (!modelHas(ma, x)) ma.push_back(x);
            if (!modelHas(mt, x)) mt.push_back(x);
        } else {
            // Write straight to the bag, behind every view's back: this is
            // allowed to create the duplicate the set would have refused.
            lbl << "insert " << x << " straight into the bag";
            ab.insert(x);
            tb.insert(x);
            ma.push_back(x);
            mt.push_back(x);
        }

        if (ma.size() > MAXSIZE) {
            ab.clear(); tb.clear();
            ma.clear(); mt.clear();
        }

        for (int k = 0; k < 3; ++k)
            if (printed(*av[k]) != renderModel(ma, false))
                fail("shared array bag: " + lbl.str(), renderModel(ma, false), printed(*av[k]));
        for (int k = 0; k < 2; ++k)
            if (printed(*tv[k]) != renderModel(mt, true))
                fail("shared tree bag: " + lbl.str(), renderModel(mt, true), printed(*tv[k]));
        if (a1.has(x) != modelHas(ma, x))
            fail("shared array bag has(): " + lbl.str(), "true", "false");
        if (t1.has(x) != modelHas(mt, x))
            fail("shared tree bag has(): " + lbl.str(), "true", "false");
    }
}

void usage(const char* prog) {
    std::cout << "usage: " << prog
              << " [--seed N] [--bags N] [--sets N] [--views N] [--random]\n";
}

} // namespace

int main(int ac, char** av) {
    unsigned seed  = 1;
    long     bags  = 4000;
    long     setsN = 4000;
    long     views = 3000;

    for (int i = 1; i < ac; ++i) {
        if (!std::strcmp(av[i], "--seed") && i + 1 < ac)
            seed = (unsigned)std::atoi(av[++i]);
        else if (!std::strcmp(av[i], "--bags") && i + 1 < ac)
            bags = std::atol(av[++i]);
        else if (!std::strcmp(av[i], "--sets") && i + 1 < ac)
            setsN = std::atol(av[++i]);
        else if (!std::strcmp(av[i], "--views") && i + 1 < ac)
            views = std::atol(av[++i]);
        else if (!std::strcmp(av[i], "--random"))
            seed = (unsigned)std::time(0);
        else {
            usage(av[0]);
            return (1);
        }
    }

    std::srand(seed);

    std::cout << vtest::cyan() << "== bag fuzz ==" << vtest::off()
              << "  seed " << seed << ", " << bags << " steps each\n";

    BagFuzzer<searchable_array_bag> af(false, "array");
    for (long i = 0; i < bags && failures == 0; ++i)
        af.step();
    if (failures == 0)
        std::cout << vtest::green() << "  [ok] " << vtest::off()
                  << "searchable_array_bag: " << af.applied << " operations, "
                  << af.skipped << " skipped (size cap)\n";

    BagFuzzer<searchable_tree_bag> tf(true, "tree");
    for (long i = 0; i < bags && failures == 0; ++i)
        tf.step();
    if (failures == 0)
        std::cout << vtest::green() << "  [ok] " << vtest::off()
                  << "searchable_tree_bag:  " << tf.applied << " operations, "
                  << tf.skipped << " skipped (size cap)\n";

    std::cout << vtest::cyan() << "== set fuzz ==" << vtest::off()
              << "  " << setsN << " steps over " << 2 * NOBJ << " sets\n";
    {
        SetFuzzer sf;
        for (long i = 0; i < setsN && failures == 0; ++i)
            sf.step();
        if (failures == 0)
            std::cout << vtest::green() << "  [ok] " << vtest::off()
                      << sf.applied << " operations, no duplicate ever entered a set\n";
    }

    std::cout << vtest::cyan() << "== shared views ==" << vtest::off()
              << "  " << views << " rounds\n";
    sharedViews(views);
    if (failures == 0)
        std::cout << vtest::green() << "  [ok] " << vtest::off()
                  << "five views over two bags stayed consistent\n";

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
