#!/bin/sh
# Checks that can only be made at compile time:
#   1. the warning matrix (several compilers x several standards)
#   2. orthodox canonical form, expressed as "this snippet must compile"
#   3. const-correctness, expressed as "this snippet must NOT compile"
#   4. the inheritance graph actually resolves (no abstract leftovers, one
#      shared virtual `bag` sub-object)
#   5. ODR: the graded headers can be included by more than one TU
#
# Only the six graded files are held to this. bag.hpp, searchable_bag.hpp,
# array_bag.* and tree_bag.* are supplied by the subject and the grader uses
# its own copies of them, so a warning that originates there is out of scope --
# see the "provided files" note at the bottom.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/polyset-compile.$$
GRADED="searchable_array_bag.cpp searchable_tree_bag.cpp set.cpp"
SOURCES="array_bag.cpp tree_bag.cpp searchable_array_bag.cpp searchable_tree_bag.cpp set.cpp"

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

fails=0
ok()   { printf '  \033[32m[ok]\033[0m %s\n' "$1"; }
bad()  { printf '  \033[1;31m[KO]\033[0m %s\n' "$1"; fails=$((fails + 1)); }

# --------------------------------------------------------------------------
printf '\n\033[36m== Warning matrix (the graded files) ==\033[0m\n'

# -Wall -Wextra -Werror -std=c++98 is what the exam uses; the rest is extra
# pressure. -Weffc++ and -Wnon-virtual-dtor are deliberately absent: both fire
# inside the provided bag.hpp / array_bag.hpp, which the graded files cannot
# change.
STRICT="-Wall -Wextra -Werror -pedantic-errors -Wshadow -Wold-style-cast -Wconversion -Wsign-conversion -Wcast-qual -Wunused"

for cxx in g++ clang++; do
    command -v $cxx >/dev/null 2>&1 || { printf '  \033[1;33m[--]\033[0m %s not installed\n' "$cxx"; continue; }
    for std in c++98 c++03 c++11 c++17; do
        log=""
        for f in $GRADED; do
            log="$log$($cxx -std=$std $STRICT -I"$SRC" -c "$SRC/$f" -o /dev/null 2>&1)"
        done
        if [ -z "$log" ]; then
            ok "$cxx -std=$std  (exam flags + shadow/conversion/pedantic)"
        else
            bad "$cxx -std=$std"
            printf '%s\n' "$log" | sed 's/^/       /' | head -12
        fi
    done
done

# The whole program, graded files and provided files together, must still
# build with exactly the exam's flags.
if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" \
       $(for f in $SOURCES; do printf '%s ' "$SRC/$f"; done) "$SRC/main.cpp" \
       -o "$WORK/subject" 2>"$WORK/all.log"; then
    ok "the whole project builds with -Wall -Wextra -Werror -std=c++98"
else
    bad "the whole project does not build with the exam's flags"
    sed 's/^/       /' "$WORK/all.log" | head -12
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== Submission: the expected files, on their own ==\033[0m\n'

# "Expected files : searchable_array_bag.cpp, searchable_array_bag.hpp,
#  searchable_tree_bag.cpp, searchable_tree_bag.hpp, set.cpp, set.hpp"
# Nothing else you wrote is collected. The six of them, dropped next to
# pristine copies of the files the subject provides, have to be a complete
# program -- so this rebuilds exactly that, in a clean directory.
SUBMIT=$WORK/submit
rm -rf "$SUBMIT"
mkdir -p "$SUBMIT"
missing=""
for f in searchable_array_bag.cpp searchable_array_bag.hpp \
         searchable_tree_bag.cpp searchable_tree_bag.hpp set.cpp set.hpp; do
    if [ -f "$SRC/$f" ]; then
        cp "$SRC/$f" "$SUBMIT/"
    else
        missing="$missing $f"
    fi
done
if [ -n "$missing" ]; then
    bad "expected file(s) missing:$missing"
else
    ok "all six expected files are present"
fi
for f in bag.hpp searchable_bag.hpp array_bag.hpp array_bag.cpp \
         tree_bag.hpp tree_bag.cpp main.cpp; do
    cp "$SRC/$f" "$SUBMIT/"            # provided by the subject, never edited
done

if (cd "$SUBMIT" && c++ -Wall -Wextra -Werror -std=c++98 *.cpp -o subject) \
        2>"$WORK/submit.log"; then
    ok "c++ -Wall -Wextra -Werror -std=c++98 *.cpp   builds with the provided main"
else
    bad "the submitted files do not build with the exam's own command"
    sed 's/^/       /' "$WORK/submit.log" | head -20
fi

if [ -x "$SUBMIT/subject" ]; then
    if "$SUBMIT/subject" 5 3 5 1 | diff -q - "$HERE/expected_subject.txt" >/dev/null; then
        ok "and the subject's main prints exactly what it should"
    else
        bad "the submission build gives different output for the subject's main"
    fi
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== orthodox canonical form ==\033[0m\n'

snippet() {
    printf '#include "searchable_array_bag.hpp"\n#include "searchable_tree_bag.hpp"\n#include "set.hpp"\nint main(){ %s ; return 0; }\n' "$1" > "$WORK/t.cpp"
}

accept() {
    snippet "$1"
    if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/t.cpp" 2>"$WORK/e.log"; then
        ok "accepted:  $1"
    else
        bad "should compile but does not:  $1"
        sed 's/^/       /' "$WORK/e.log" | head -8
    fi
}

reject() {
    snippet "$1"
    if c++ -std=c++98 -fsyntax-only -I"$SRC" "$WORK/t.cpp" 2>/dev/null; then
        bad "compiles but should not:  $1"
    else
        ok "rejected:  $1"
    fi
}

# Default ctor, copy ctor, copy assignment, destructor -- for both bags.
accept 'searchable_array_bag a; searchable_array_bag b(a); searchable_array_bag c; c = a;'
accept 'searchable_tree_bag a; searchable_tree_bag b(a); searchable_tree_bag c; c = a;'
accept 'const searchable_array_bag a; searchable_array_bag b(a); searchable_array_bag c; c = a;'
accept 'const searchable_tree_bag a; searchable_tree_bag b(a); searchable_tree_bag c; c = a;'
# The assignment operators must return a reference, so they can be chained.
accept 'searchable_array_bag a, b, c; a = b = c; searchable_array_bag& r = (a = b); (void)r;'
accept 'searchable_tree_bag a, b, c; a = b = c; searchable_tree_bag& r = (a = b); (void)r;'
# set wraps a bag; the copy and the assignment are views onto the same bag.
accept 'searchable_array_bag a; set s(a); set t(s); set u(a); set& r = (u = s); (void)r; (void)t;'
accept 'searchable_tree_bag a; set s(a); set t(s); (void)t;'
# set must accept anything that is a searchable_bag, not just one of the two.
accept 'searchable_array_bag a; searchable_bag& r = a; set s(r); (void)s;'

# --------------------------------------------------------------------------
printf '\n\033[36m== const-correctness ==\033[0m\n'

# Everything that only reads must be callable on a const object.
accept 'const searchable_array_bag a; a.print(); (void)a.has(1);'
accept 'const searchable_tree_bag a; a.print(); (void)a.has(1);'
accept 'searchable_array_bag b; const set s(b); s.print(); (void)s.has(1); (void)s.get_bag();'
accept 'const searchable_array_bag a; const bag& g = a; g.print();'
accept 'const searchable_tree_bag a; const searchable_bag& r = a; (void)r.has(1);'
# and nothing that writes may be.
reject 'const searchable_array_bag a; a.insert(1);'
reject 'const searchable_tree_bag a; a.insert(1);'
reject 'const searchable_array_bag a; a.clear();'
reject 'const searchable_tree_bag a; a.clear();'
reject 'searchable_array_bag b; const set s(b); s.insert(1);'
reject 'searchable_array_bag b; const set s(b); s.clear();'
reject 'const searchable_array_bag a; int arr[1] = {0}; a.insert(arr, 1);'
# The bag a set wraps is private state.
reject 'searchable_array_bag b; set s(b); s._bag = 0;'

# --------------------------------------------------------------------------
printf '\n\033[36m== the inheritance graph ==\033[0m\n'

# Neither class may be left abstract: every pure virtual in bag and
# searchable_bag must have a final overrider.
accept 'searchable_array_bag a; (void)a;'
accept 'searchable_tree_bag a; (void)a;'
# Both must be usable through each abstract interface.
accept 'searchable_array_bag a; bag* p = &a; p->insert(1); p->print(); p->clear();'
accept 'searchable_tree_bag a; bag* p = &a; p->insert(1); p->print(); p->clear();'
accept 'searchable_array_bag a; searchable_bag* p = &a; p->insert(1); (void)p->has(1);'
accept 'searchable_tree_bag a; searchable_bag* p = &a; p->insert(1); (void)p->has(1);'
# Both overloads of insert must remain visible in the derived classes: a
# `using` omitted here is the classic way to hide insert(int*, int).
accept 'searchable_array_bag a; int arr[2] = {1,2}; a.insert(arr, 2); a.insert(3);'
accept 'searchable_tree_bag a; int arr[2] = {1,2}; a.insert(arr, 2); a.insert(3);'
# `bag` is a virtual base, so there is exactly one sub-object and the cast is
# unambiguous. With non-virtual inheritance this line is an error.
accept 'searchable_array_bag a; bag* p = &a; (void)p;'
accept 'searchable_tree_bag a; bag* p = &a; (void)p;'
# ... and the cross-cast back down works, which needs a polymorphic type.
accept 'searchable_array_bag a; bag* p = &a; (void)dynamic_cast<searchable_bag*>(p);'
# set stores a searchable_bag, so a plain bag is not enough.
reject 'array_bag a; set s(a); (void)s;'
reject 'searchable_array_bag a; bag& g = a; set s(g); (void)s;'

printf '\n\033[36m== there is exactly one bag sub-object ==\033[0m\n'
cat > "$WORK/virt.cpp" <<'EOF'
#include "searchable_array_bag.hpp"
#include "searchable_tree_bag.hpp"
#include <iostream>
int main() {
    searchable_array_bag a;
    searchable_tree_bag  t;
    // Reached through the array_bag side and the searchable_bag side: with a
    // non-virtual `bag` these would be two different sub-objects and the two
    // inserts would land in two different places.
    bag* viaArray = static_cast<array_bag*>(&a);
    bag* viaSearchable = static_cast<searchable_bag*>(&a);
    if (viaArray != viaSearchable) return (1);
    bag* viaTree = static_cast<tree_bag*>(&t);
    if (viaTree != static_cast<searchable_bag*>(&t)) return (2);
    viaArray->insert(1);
    if (!a.has(1)) return (3);
    viaTree->insert(1);
    if (!t.has(1)) return (4);
    std::cout << "  one shared bag sub-object in both classes\n";
    return (0);
}
EOF
if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" \
       "$SRC/array_bag.cpp" "$SRC/tree_bag.cpp" "$WORK/virt.cpp" -o "$WORK/virt" 2>"$WORK/virt.log"; then
    if "$WORK/virt"; then
        ok 'the virtual bag base resolves to a single sub-object'
    else
        bad "the two inheritance paths reach different bag sub-objects"
    fi
else
    bad "the virtual-base check does not compile"
    sed 's/^/       /' "$WORK/virt.log" | head -12
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== ODR: headers included from two translation units ==\033[0m\n'

# searchable_array_bag, searchable_tree_bag and set are defined entirely
# inside their headers, so every member is implicitly inline. Including them
# from several TUs and linking is what proves it.
cat > "$WORK/tu_a.cpp" <<'EOF'
#include "searchable_array_bag.hpp"
#include "searchable_tree_bag.hpp"
#include "set.hpp"
void fillA(set& s) { int arr[] = {1, 2, 2, 3}; s.insert(arr, 4); }
bool probeA(const set& s, int v) { return (s.has(v)); }
EOF

cat > "$WORK/tu_b.cpp" <<'EOF'
#include "searchable_array_bag.hpp"
#include "searchable_tree_bag.hpp"
#include "set.hpp"
#include <iostream>
void fillA(set& s);
bool probeA(const set& s, int v);
int main() {
    searchable_array_bag a;
    set s(a);
    fillA(s);
    s.insert(2);
    if (!probeA(s, 1) || !probeA(s, 3)) return (1);
    if (probeA(s, 9))                   return (2);
    searchable_tree_bag t;
    set u(t);
    fillA(u);
    if (!u.has(2))                      return (3);
    std::cout << "  both TUs agree: ";
    s.print();
    return (0);
}
EOF

if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" \
       "$SRC/array_bag.cpp" "$SRC/tree_bag.cpp" "$SRC/searchable_array_bag.cpp" \
       "$SRC/searchable_tree_bag.cpp" "$SRC/set.cpp" \
       "$WORK/tu_a.cpp" "$WORK/tu_b.cpp" -o "$WORK/odr" 2>"$WORK/odr.log"; then
    if "$WORK/odr"; then
        ok "links and runs with the graded headers in 4 translation units"
    else
        bad "multi-TU program ran but produced the wrong answer"
    fi
else
    bad "multi-TU link failed (a member defined out of line without inline?)"
    sed 's/^/       /' "$WORK/odr.log" | head -12
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== the graded files stand on their own ==\033[0m\n'

for f in searchable_array_bag searchable_tree_bag set; do
    if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" -c "$SRC/$f.cpp" -o "$WORK/$f.o" 2>/dev/null; then
        if nm -C "$WORK/$f.o" 2>/dev/null | grep -q ' T main'; then
            bad "$f.cpp defines main() -- it would clash with the grader's main"
        else
            ok "$f.cpp compiles on its own and defines no main()"
        fi
    else
        bad "$f.cpp does not compile on its own"
    fi

    printf '#include "%s.hpp"\n' "$f" > "$WORK/solo.cpp"
    if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/solo.cpp" 2>/dev/null; then
        ok "$f.hpp is self-contained (compiles with nothing included before it)"
    else
        bad "$f.hpp needs another header to be included first"
    fi

    printf '#include "%s.hpp"\n#include "%s.hpp"\nint main(){ return 0; }\n' "$f" "$f" > "$WORK/twice.cpp"
    if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/twice.cpp" 2>/dev/null; then
        ok "$f.hpp is safe to include twice (include guard works)"
    else
        bad "including $f.hpp twice breaks the build"
    fi
done

# --------------------------------------------------------------------------
printf '\n\033[36m== provided files (not graded, reported for information) ==\033[0m\n'

# array_bag allocates with new[] and frees with plain delete, in three places.
# Nothing in the graded files can fix that, and the grader uses its own copy
# of array_bag.cpp, so it is reported rather than failed.
n=$(grep -c 'delete this->data' "$SRC/array_bag.cpp" 2>/dev/null || echo 0)
if [ "$n" -gt 0 ]; then
    printf '  \033[1;33m[--]\033[0m array_bag.cpp frees a new[] array with plain delete (%s sites)\n' "$n"
    printf '        ASan and valgrind report this; see tests/README.md.\n'
else
    ok "array_bag.cpp frees its array with delete[]"
fi

if grep -q 'virtual ~bag' "$SRC/bag.hpp" 2>/dev/null; then
    ok "bag has a virtual destructor"
else
    printf '  \033[1;33m[--]\033[0m bag has no virtual destructor: deleting through bag* is undefined\n'
    printf '        The subject main never deletes, and the graded files cannot add it.\n'
fi

echo
echo "-----------------------------"
if [ $fails -eq 0 ]; then
    printf '\033[32mall compile-time checks passed\033[0m\n'
    exit 0
fi
printf '\033[1;31m%d compile-time check(s) failed\033[0m\n' "$fails"
exit 1
