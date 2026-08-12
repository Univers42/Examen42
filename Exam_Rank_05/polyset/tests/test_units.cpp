#include "../searchable_array_bag.hpp"
#include "../searchable_tree_bag.hpp"
#include "../set.hpp"
#include "vtest.hpp"
#include <climits>
#include <sstream>
#include <string>

using vtest::check;
using vtest::assertTrue;
using vtest::section;

// print() writes to std::cout, so the only way to test what a bag contains is
// to steal the stream buffer for the duration of the call.
static std::string printed(const bag& b) {
    std::ostringstream os;
    std::streambuf* old = std::cout.rdbuf(os.rdbuf());
    b.print();
    std::cout.rdbuf(old);
    return (os.str());
}

static std::string printed(const set& s) {
    std::ostringstream os;
    std::streambuf* old = std::cout.rdbuf(os.rdbuf());
    s.print();
    std::cout.rdbuf(old);
    return (os.str());
}

// The format the provided array_bag/tree_bag use: every value followed by a
// space, then a newline. Expectations are written as value lists so the tests
// never hard-code that format in more than this one place.
static std::string lineOf(const int* v, int n) {
    std::ostringstream os;
    for (int i = 0; i < n; ++i)
        os << v[i] << " ";
    os << "\n";
    return (os.str());
}
static std::string line() { return ("\n"); }
static std::string line(int a) { return (lineOf(&a, 1)); }
static std::string line(int a, int b) {
    int v[] = { a, b }; return (lineOf(v, 2));
}
static std::string line(int a, int b, int c) {
    int v[] = { a, b, c }; return (lineOf(v, 3));
}
static std::string line(int a, int b, int c, int d) {
    int v[] = { a, b, c, d }; return (lineOf(v, 4));
}
static std::string line(int a, int b, int c, int d, int e) {
    int v[] = { a, b, c, d, e }; return (lineOf(v, 5));
}
static std::string line(int a, int b, int c, int d, int e, int f) {
    int v[] = { a, b, c, d, e, f }; return (lineOf(v, 6));
}

// ---------------------------------------------------------------------------

static void testArrayBagBasics() {
    section("searchable_array_bag: storage and order");

    searchable_array_bag b;
    check(line(), printed(b), "a fresh bag prints an empty line");
    assertTrue(!b.has(0), "empty bag has nothing");
    assertTrue(!b.has(42), "empty bag has nothing (2)");

    b.insert(5);
    check(line(5), printed(b), "one value");
    b.insert(3);
    b.insert(9);
    check(line(5, 3, 9), printed(b), "an array bag keeps insertion order");

    assertTrue(b.has(5), "has(5)");
    assertTrue(b.has(3), "has(3)");
    assertTrue(b.has(9), "has(9)");
    assertTrue(!b.has(4), "!has(4)");
    assertTrue(!b.has(0), "!has(0)");

    // A bag, unlike a set, keeps duplicates.
    b.insert(5);
    check(line(5, 3, 9, 5), printed(b), "duplicates are kept");
    assertTrue(b.has(5), "has(5) with two copies present");

    section("searchable_array_bag: bulk insert");

    searchable_array_bag c;
    int arr[] = { 1, 2, 3, 4 };
    c.insert(arr, 4);
    check(line(1, 2, 3, 4), printed(c), "insert(arr, 4)");

    c.clear();
    c.insert(arr, 0);
    check(line(), printed(c), "insert(arr, 0) inserts nothing");

    c.insert(arr, 2);
    check(line(1, 2), printed(c), "insert(arr, 2) inserts a prefix");
    assertTrue(c.has(1) && c.has(2), "the prefix is really there");
    assertTrue(!c.has(3) && !c.has(4), "the rest is not");

    // The source array must not be touched.
    check(1, arr[0], "insert(arr, n) does not modify the array");
    check(4, arr[3], "insert(arr, n) does not modify the array (2)");

    section("searchable_array_bag: clear");

    searchable_array_bag d;
    d.insert(7);
    d.insert(8);
    d.clear();
    check(line(), printed(d), "clear empties the bag");
    assertTrue(!d.has(7), "has() is false after clear");
    d.clear();
    check(line(), printed(d), "clear twice is safe");
    d.insert(1);
    check(line(1), printed(d), "the bag is reusable after clear");
}

static void testTreeBagBasics() {
    section("searchable_tree_bag: storage and order");

    searchable_tree_bag b;
    check(line(), printed(b), "a fresh bag prints an empty line");
    assertTrue(!b.has(0), "empty bag has nothing");

    b.insert(5);
    check(line(5), printed(b), "one value");
    b.insert(3);
    b.insert(9);
    check(line(3, 5, 9), printed(b), "a tree bag prints in sorted order");

    b.clear();
    b.insert(9);
    b.insert(3);
    b.insert(5);
    check(line(3, 5, 9), printed(b), "sorted whatever the insertion order");

    assertTrue(b.has(3) && b.has(5) && b.has(9), "has() finds every value");
    assertTrue(!b.has(4), "!has(4)");
    assertTrue(!b.has(2), "!has(2)  (left of everything)");
    assertTrue(!b.has(10), "!has(10) (right of everything)");

    section("searchable_tree_bag: duplicates");

    searchable_tree_bag dup;
    dup.insert(5);
    dup.insert(5);
    dup.insert(5);
    check(line(5, 5, 5), printed(dup), "three copies of one value");
    assertTrue(dup.has(5), "has() with duplicates");
    assertTrue(!dup.has(6), "!has(6) past a chain of duplicates");
    assertTrue(!dup.has(4), "!has(4)");

    // Duplicates go to the right, so has() must not stop at the first node
    // that is merely >= the target.
    searchable_tree_bag mixed;
    int vals[] = { 5, 5, 3, 7, 5, 3 };
    mixed.insert(vals, 6);
    check(line(3, 3, 5, 5, 5, 7), printed(mixed), "duplicates of two values, sorted");
    assertTrue(mixed.has(3) && mixed.has(5) && mixed.has(7), "every distinct value is found");
    assertTrue(!mixed.has(6), "!has(6)");

    section("searchable_tree_bag: degenerate shapes");

    // Ascending inserts make a right-leaning chain, descending a left-leaning
    // one. has() must still reach the far end of both.
    searchable_tree_bag asc;
    for (int i = 0; i < 200; ++i)
        asc.insert(i);
    assertTrue(asc.has(0), "right-leaning chain: has(first)");
    assertTrue(asc.has(199), "right-leaning chain: has(last)");
    assertTrue(!asc.has(200), "right-leaning chain: !has(past the end)");
    assertTrue(!asc.has(-1), "right-leaning chain: !has(before the start)");

    searchable_tree_bag desc;
    for (int i = 199; i >= 0; --i)
        desc.insert(i);
    assertTrue(desc.has(0), "left-leaning chain: has(first)");
    assertTrue(desc.has(199), "left-leaning chain: has(last)");
    assertTrue(!desc.has(200), "left-leaning chain: !has(past the end)");

    std::string a = printed(asc), d = printed(desc);
    check(a, d, "both insertion orders print the same sorted line");

    section("searchable_tree_bag: clear");

    searchable_tree_bag c;
    c.insert(1);
    c.insert(2);
    c.clear();
    check(line(), printed(c), "clear empties the tree");
    assertTrue(!c.has(1), "has() is false after clear");
    c.clear();
    check(line(), printed(c), "clear twice is safe");
    c.insert(4);
    check(line(4), printed(c), "the tree is reusable after clear");
}

static void testExtremes() {
    section("Extreme values");

    const int vals[] = { INT_MIN, -1, 0, 1, INT_MAX };

    searchable_array_bag a;
    searchable_tree_bag t;
    for (int i = 0; i < 5; ++i) {
        a.insert(vals[i]);
        t.insert(vals[i]);
    }
    for (int i = 0; i < 5; ++i) {
        std::ostringstream lbl;
        lbl << "array bag has(" << vals[i] << ")";
        assertTrue(a.has(vals[i]), lbl.str());
        std::ostringstream lbl2;
        lbl2 << "tree bag has(" << vals[i] << ")";
        assertTrue(t.has(vals[i]), lbl2.str());
    }
    assertTrue(!a.has(INT_MAX - 1), "array bag !has(INT_MAX - 1)");
    assertTrue(!t.has(INT_MAX - 1), "tree bag !has(INT_MAX - 1)");
    assertTrue(!t.has(INT_MIN + 1), "tree bag !has(INT_MIN + 1)");

    // INT_MIN sorts first and INT_MAX last, in the tree's own order.
    std::ostringstream want;
    want << INT_MIN << " " << -1 << " " << 0 << " " << 1 << " " << INT_MAX << " \n";
    check(want.str(), printed(t), "tree bag sorts the whole int range");
}

// The canonical form is where the provided base classes are at their most
// hostile: both of them clear() before they copy, so an unguarded
// self-assignment destroys the object.
template <typename Bag>
static void testCanonicalForm(const std::string& what) {
    section(what + ": orthodox canonical form");

    Bag src;
    src.insert(1);
    src.insert(2);
    src.insert(3);
    const std::string full = printed(src);

    Bag cpy(src);
    check(full, printed(cpy), "copy ctor reproduces the contents");
    assertTrue(&cpy != &src, "copy ctor makes a distinct object");

    // Deep, not shared: mutating either side must not reach the other.
    cpy.insert(4);
    check(full, printed(src), "source unchanged after mutating the copy");
    assertTrue(cpy.has(4) && !src.has(4), "the new value lives only in the copy");

    src.insert(9);
    assertTrue(src.has(9) && !cpy.has(9), "and the other way round");
    src.clear();
    assertTrue(cpy.has(1), "clearing the source leaves the copy intact");

    Bag a;
    a.insert(7);
    Bag b;
    b.insert(8);
    b.insert(9);
    Bag& ret = (b = a);
    check(printed(a), printed(b), "operator= reproduces the contents");
    assertTrue(&ret == &b, "operator= returns *this (not a copy)");
    assertTrue(!b.has(8) && !b.has(9), "operator= replaces, it does not append");

    b.insert(100);
    assertTrue(!a.has(100), "assignment is a deep copy");

    // Both provided bases implement operator= as "clear, then copy from the
    // source", which wipes the object when the source *is* the object.
    Bag s;
    s.insert(11);
    s.insert(22);
    const std::string before = printed(s);
    s = s;
    check(before, printed(s), "self-assignment  s = s  preserves the contents");
    assertTrue(s.has(11) && s.has(22), "  and has() still finds them");

    Bag& alias = s;
    s = alias;
    check(before, printed(s), "self-assignment through an alias");

    // Assigning an empty bag must actually empty the target.
    Bag empty;
    Bag target;
    target.insert(5);
    target = empty;
    check(line(), printed(target), "assigning an empty bag empties the target");
    assertTrue(!target.has(5), "  and has() agrees");

    // (a = b) is an lvalue, so it can be assigned again, and chains must work.
    Bag t1, t2, t3;
    t2.insert(2);
    t3.insert(3);
    (t1 = t2) = t3;
    check(printed(t3), printed(t1), "(t1 = t2) = t3");

    Bag c1, c2, c3;
    c3.insert(42);
    c1 = c2 = c3;
    check(printed(c3), printed(c1), "chained  c1 = c2 = c3");

    // A const bag must still be readable.
    Bag filled;
    filled.insert(1);
    const Bag konst(filled);
    check(printed(filled), printed(konst), "a const bag prints");
    assertTrue(konst.has(1), "a const bag answers has()");
    assertTrue(!konst.has(2), "a const bag answers has() negatively too");
}

static void testPolymorphism() {
    section("Polymorphism");

    searchable_array_bag ab;
    searchable_tree_bag tb;

    // Through the abstract searchable_bag, which is what set holds.
    searchable_bag* s[2];
    s[0] = &ab;
    s[1] = &tb;
    for (int i = 0; i < 2; ++i) {
        s[i]->insert(4);
        s[i]->insert(2);
        assertTrue(s[i]->has(4), "has() dispatches through searchable_bag*");
        assertTrue(!s[i]->has(3), "!has() dispatches through searchable_bag*");
    }
    check(line(4, 2), printed(ab), "insert dispatched to the array bag");
    check(line(2, 4), printed(tb), "insert dispatched to the tree bag");

    // Through the plain bag base: the virtual bag is shared, so there is only
    // one sub-object and both paths must reach the same state.
    bag* b0 = &ab;
    bag* b1 = &tb;
    b0->insert(6);
    b1->insert(6);
    assertTrue(ab.has(6), "insert through bag* reaches the array bag");
    assertTrue(tb.has(6), "insert through bag* reaches the tree bag");

    int arr[] = { 8, 9 };
    b0->insert(arr, 2);
    b1->insert(arr, 2);
    assertTrue(ab.has(8) && ab.has(9), "insert(int*, int) through bag*");
    assertTrue(tb.has(8) && tb.has(9), "insert(int*, int) through bag* (tree)");

    check(line(4, 2, 6, 8, 9), printed(*b0), "print through bag* (array)");
    check(line(2, 4, 6, 8, 9), printed(*b1), "print through bag* (tree)");

    b0->clear();
    b1->clear();
    check(line(), printed(ab), "clear through bag* (array)");
    check(line(), printed(tb), "clear through bag* (tree)");

    // The virtual base means a searchable_bag* and a bag* denote the same
    // object even though the addresses may differ.
    searchable_bag* sp = &ab;
    bag* bp = sp;
    sp->insert(1);
    assertTrue(dynamic_cast<searchable_array_bag*>(bp) == &ab,
               "bag* cross-casts back to the same object");
    assertTrue(dynamic_cast<searchable_bag*>(bp) == sp,
               "bag* cross-casts to the searchable interface");
    assertTrue(dynamic_cast<searchable_tree_bag*>(bp) == 0,
               "and not to the wrong derived type");
}

static void testSet() {
    section("set: wrapping an array bag");

    searchable_array_bag ab;
    set s(ab);

    check(line(), printed(s), "a fresh set prints an empty line");
    assertTrue(!s.has(1), "empty set has nothing");

    s.insert(5);
    s.insert(3);
    check(line(5, 3), printed(s), "insertion order is the bag's own");

    // The point of the class: a second insert of the same value is dropped.
    s.insert(5);
    check(line(5, 3), printed(s), "inserting a duplicate changes nothing");
    s.insert(3);
    s.insert(5);
    check(line(5, 3), printed(s), "and still nothing after several");

    assertTrue(s.has(5) && s.has(3), "has() finds what was inserted");
    assertTrue(!s.has(4), "!has(4)");

    s.insert(4);
    check(line(5, 3, 4), printed(s), "a genuinely new value is inserted");

    section("set: wrapping a tree bag");

    searchable_tree_bag tb;
    set t(tb);
    t.insert(5);
    t.insert(3);
    t.insert(5);
    t.insert(9);
    t.insert(3);
    check(line(3, 5, 9), printed(t), "a tree-backed set prints sorted and unique");
    assertTrue(t.has(9) && !t.has(4), "has() on a tree-backed set");

    section("set: bulk insert");

    searchable_array_bag ab2;
    set s2(ab2);
    int arr[] = { 1, 2, 2, 3, 1, 4 };
    s2.insert(arr, 6);
    check(line(1, 2, 3, 4), printed(s2), "insert(arr, 6) drops the repeats");
    check(1, arr[0], "insert(arr, n) does not modify the array");
    check(2, arr[2], "insert(arr, n) does not modify the array (2)");

    s2.insert(arr, 6);
    check(line(1, 2, 3, 4), printed(s2), "inserting the same array again is a no-op");

    s2.insert(arr, 0);
    check(line(1, 2, 3, 4), printed(s2), "insert(arr, 0) is a no-op");

    section("set: clear");

    s2.clear();
    check(line(), printed(s2), "clear empties the set");
    assertTrue(!s2.has(1), "has() is false after clear");
    check(line(), printed(ab2), "  and the wrapped bag really is empty");
    s2.insert(1);
    check(line(1), printed(s2), "the set is reusable after clear");

    section("set: the wrapped bag");

    searchable_array_bag ab3;
    set s3(ab3);
    s3.insert(7);
    check(printed(ab3), printed(s3), "set::print is the bag's print");
    check(printed(ab3), printed(s3.get_bag()), "get_bag() hands back the same bag");
    assertTrue(&s3.get_bag() == static_cast<searchable_bag*>(&ab3),
               "get_bag() returns the very object that was wrapped");

    // The set is a view, not an owner: writing through the bag is visible
    // through the set, duplicates included.
    ab3.insert(7);
    check(line(7, 7), printed(s3), "a duplicate inserted behind the set's back shows up");
    assertTrue(s3.has(7), "has() still answers");

    section("set: canonical form");

    searchable_array_bag ab4;
    set a(ab4);
    a.insert(1);

    set cpy(a);
    check(printed(a), printed(cpy), "copy ctor");
    assertTrue(&cpy.get_bag() == &a.get_bag(), "a copied set wraps the same bag");
    cpy.insert(2);
    check(printed(a), printed(cpy), "  so both views see the insert");

    searchable_tree_bag other;
    set b(other);
    set& ret = (b = a);
    assertTrue(&ret == &b, "operator= returns *this (not a copy)");
    assertTrue(&b.get_bag() == &a.get_bag(), "operator= rebinds to the other bag");
    check(printed(a), printed(b), "  and prints the same contents");

    set sa(ab4);
    set& alias = sa;
    sa = sa;
    check(printed(a), printed(sa), "self-assignment  s = s");
    sa = alias;
    check(printed(a), printed(sa), "self-assignment through an alias");

    // Two sets over one bag: the dedup rule is the bag's contents, so an
    // insert through one is seen by the other.
    searchable_array_bag shared;
    set v1(shared), v2(shared);
    v1.insert(3);
    v2.insert(3);
    check(line(3), printed(shared), "two sets over one bag do not double-insert");

    // A const set must still be readable. `set` is a view onto someone
    // else's bag, so every one of these is const: constness of the view says
    // nothing about the bag behind it.
    const set konst(a);
    assertTrue(konst.has(1), "a const set answers has()");
    check(printed(a), printed(konst), "a const set prints");
    check(printed(a), printed(konst.get_bag()), "a const set hands back its bag");
    assertTrue(&konst.get_bag() == &a.get_bag(), "  and it is the same bag");
}

static void testSubjectSequence() {
    section("The subject main's sequence, on live objects");

    // Exactly what ../main.cpp does, with the arguments {5, 3, 5, 1}, checked
    // statement by statement instead of as one captured blob.
    const int av[] = { 5, 3, 5, 1 };

    // The subject allocates these with `new` and never deletes them; the
    // provided `bag` has no virtual destructor, so deleting through a
    // searchable_bag* would be undefined. Automatic storage sidesteps a
    // limitation of the given files that the graded files cannot fix.
    searchable_tree_bag treeStorage;
    searchable_array_bag arrayStorage;
    searchable_bag* t = &treeStorage;
    searchable_bag* a = &arrayStorage;
    for (int i = 0; i < 4; ++i) {
        t->insert(av[i]);
        a->insert(av[i]);
    }
    check(line(1, 3, 5, 5), printed(*t), "tree bag after the insert loop");
    check(line(5, 3, 5, 1), printed(*a), "array bag after the insert loop");

    for (int i = 0; i < 4; ++i) {
        // v - 1 is 4, 2, 4 and 0 here, none of which was inserted.
        assertTrue(t->has(av[i]), "t->has(v)");
        assertTrue(a->has(av[i]), "a->has(v)");
        assertTrue(!t->has(av[i] - 1), "!t->has(v - 1)");
        assertTrue(!a->has(av[i] - 1), "!a->has(v - 1)");
    }

    t->clear();
    a->clear();
    check(line(), printed(*t), "tree bag cleared");
    check(line(), printed(*a), "array bag cleared");

    // The subject copy-constructs a const bag out of a downcast reference.
    const searchable_array_bag tmp(static_cast<searchable_array_bag&>(*a));
    check(line(), printed(tmp), "const copy of the cleared array bag");
    assertTrue(!tmp.has(1), "const copy answers has()");

    set sa(*a);
    set st(*a);                 // both views wrap the same bag, as in the subject
    for (int i = 0; i < 4; ++i) {
        st.insert(av[i]);
        sa.insert(av[i]);
    }
    check(line(5, 3, 1), printed(sa), "the two views dedup against one bag");
    check(printed(sa), printed(sa.get_bag()), "get_bag() prints the same");
    check(printed(sa), printed(st), "the other view prints the same");

    sa.clear();
    check(line(), printed(sa), "cleared through one view");
    int arr[] = { 1, 2, 3, 4 };
    sa.insert(arr, 4);
    check(line(1, 2, 3, 4), printed(sa), "insert(arr, 4) after the clear");
}

int main(int ac, char** av) {
    if (ac > 1 && std::string(av[1]) == "-v")
        vtest::verbose() = true;

    testArrayBagBasics();
    testTreeBagBasics();
    testExtremes();
    testCanonicalForm<searchable_array_bag>("searchable_array_bag");
    testCanonicalForm<searchable_tree_bag>("searchable_tree_bag");
    testPolymorphism();
    testSet();
    testSubjectSequence();

    return (vtest::summary("checks"));
}
