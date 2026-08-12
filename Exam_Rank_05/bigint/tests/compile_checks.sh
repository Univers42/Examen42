#!/bin/sh
# Checks that can only be made at compile time:
#   1. the warning matrix (several compilers x several standards)
#   2. const-correctness, expressed as "this snippet must NOT compile"
#   3. the conversions the subject's main relies on
#   4. ODR: the header can be included by more than one translation unit
#
# Almost all of bigint lives in the header -- the constructors, both increments
# and every friend operator are defined inside the class body, so they are
# implicitly inline. Check 4 is what proves that is really the case.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/bigint-compile.$$

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

fails=0
ok()   { printf '  \033[32m[ok]\033[0m %s\n' "$1"; }
bad()  { printf '  \033[1;31m[KO]\033[0m %s\n' "$1"; fails=$((fails + 1)); }

# --------------------------------------------------------------------------
printf '\n\033[36m== Warning matrix ==\033[0m\n'

# -Wall -Wextra -Werror -std=c++98 is what the exam uses; the rest is extra
# pressure.
STRICT="-Wall -Wextra -Werror -pedantic-errors -Wshadow -Wold-style-cast -Wconversion -Wsign-conversion -Wcast-qual -Wunused -Weffc++"

for cxx in g++ clang++; do
    command -v $cxx >/dev/null 2>&1 || { printf '  \033[1;33m[--]\033[0m %s not installed\n' "$cxx"; continue; }
    for std in c++98 c++03 c++11 c++17; do
        log=$($cxx -std=$std $STRICT -c "$SRC/bigint.cpp" -o /dev/null 2>&1)
        if [ -z "$log" ]; then
            ok "$cxx -std=$std  (exam flags + shadow/conversion/effc++)"
        else
            bad "$cxx -std=$std"
            printf '%s\n' "$log" | sed 's/^/       /' | head -12
        fi
    done
done

# --------------------------------------------------------------------------
printf '\n\033[36m== Submission: the expected files, on their own ==\033[0m\n'

# "Expected files : bigint.hpp, bigint.cpp" -- nothing else you wrote is
# collected, so those two plus the main the subject ships have to be a
# complete program. Rebuilding them in a clean directory is what proves it.
SUBMIT=$WORK/submit
rm -rf "$SUBMIT"
mkdir -p "$SUBMIT"
missing=""
for f in bigint.hpp bigint.cpp; do
    if [ -f "$SRC/$f" ]; then
        cp "$SRC/$f" "$SUBMIT/"
    else
        missing="$missing $f"
    fi
done
if [ -n "$missing" ]; then
    bad "expected file(s) missing:$missing"
else
    ok "both expected files are present: bigint.hpp bigint.cpp"
fi
cp "$SRC/main.cpp" "$SUBMIT/"          # the main the subject ships with the exercise

if (cd "$SUBMIT" && c++ -Wall -Wextra -Werror -std=c++98 bigint.cpp main.cpp -o subject) \
        2>"$WORK/submit.log"; then
    ok "c++ -Wall -Wextra -Werror -std=c++98 bigint.cpp main.cpp   builds"
else
    bad "the submitted files do not build with the exam's own command"
    sed 's/^/       /' "$WORK/submit.log" | head -20
fi

if [ -x "$SUBMIT/subject" ]; then
    if "$SUBMIT/subject" | diff -q - "$HERE/expected_subject.txt" >/dev/null; then
        ok "and the subject's main prints exactly what it should"
    else
        bad "the submission build gives different output for the subject's main"
    fi
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== const-correctness (must be rejected) ==\033[0m\n'

reject() {
    printf '#include "bigint.hpp"\nint main(){ %s ; return 0; }\n' "$1" > "$WORK/t.cpp"
    if c++ -std=c++98 -fsyntax-only -I"$SRC" "$WORK/t.cpp" 2>/dev/null; then
        bad "compiles but should not:  $1"
    else
        ok "rejected:  $1"
    fi
}

accept() {
    printf '#include "bigint.hpp"\nint main(){ %s ; return 0; }\n' "$1" > "$WORK/t.cpp"
    if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/t.cpp" 2>"$WORK/e.log"; then
        ok "accepted:  $1"
    else
        bad "should compile but does not:  $1"
        sed 's/^/       /' "$WORK/e.log" | head -8
    fi
}

# The subject declares `const bigint a(42)`, so every mutating operation has
# to be unavailable on it.
reject 'const bigint a(42); a += 1;'
reject 'const bigint a(42); ++a;'
reject 'const bigint a(42); a++;'
reject 'const bigint a(42); a <<= 1;'
reject 'const bigint a(42); a >>= 1;'
reject 'const bigint a(42); a = bigint(1);'
# The digit string ctor is explicit: it must not fire on its own.
reject 'bigint x = std::string("42"); (void)x;'
reject 'std::string s("42"); bigint x; x = s; (void)x;'
# The digit string is private state, not a public field.
reject 'bigint x(42); x.s = "1";'

printf '\n\033[36m== every operation a const bigint must still support ==\033[0m\n'
accept 'const bigint a(42); bigint b(21); (void)(a + b); (void)(b + a);'
accept 'const bigint a(42); (void)(a << 3); (void)(a >> 1);'
accept 'const bigint a(42); bigint b(21); (void)(a < b); (void)(a <= b); (void)(a > b);'
accept 'const bigint a(42); bigint b(21); (void)(a >= b); (void)(a == b); (void)(a != b);'
accept 'const bigint a(42); std::cout << a;'
accept 'const bigint a(42); bigint copy(a); bigint z; z = a; (void)copy;'
accept 'bigint d(1337); (void)(d >>= (const bigint)2);'   # exactly as in the subject main

printf '\n\033[36m== the conversions the subject main relies on ==\033[0m\n'
accept 'bigint b = 42; (void)b;'                        # implicit int -> bigint
accept 'bigint b(21); (void)((b << 10) + 42);'          # int as an operand of +
accept 'bigint b(21); (void)(42 + b);'                  # int on the left
accept 'bigint b(21); b <<= 4; b >>= 2;'                # int as a shift amount
accept 'bigint b(21); (void)(b == 21); (void)(21 == b);'
accept 'bigint x((std::string("1234")));'               # explicit, spelled out

# --------------------------------------------------------------------------
printf '\n\033[36m== ODR: header included from two translation units ==\033[0m\n'

cat > "$WORK/tu_a.cpp" <<'EOF'
#include "bigint.hpp"
#include <sstream>
#include <string>
bigint buildA() { return ((bigint(42) << 3) + 1337); }
std::string renderA(const bigint& b) { std::ostringstream os; os << b; return (os.str()); }
EOF

cat > "$WORK/tu_b.cpp" <<'EOF'
#include "bigint.hpp"
#include <sstream>
#include <string>
bigint buildA();
std::string renderA(const bigint& b);
int main() {
    bigint local = bigint(1337) >> 2;
    std::ostringstream os;
    os << local;
    if (!(buildA() == bigint(std::string("43337")))) return (1);
    if (renderA(local) != "13")                      return (2);
    if (os.str() != renderA(local))                  return (3);
    if (!(buildA() > local))                         return (4);
    std::cout << "  both TUs agree: " << buildA() << " and " << local << "\n";
    return (0);
}
EOF

if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" \
       "$SRC/bigint.cpp" "$WORK/tu_a.cpp" "$WORK/tu_b.cpp" -o "$WORK/odr" 2>"$WORK/odr.log"; then
    if "$WORK/odr"; then
        ok "links and runs with the header in 3 translation units"
    else
        bad "multi-TU program ran but produced the wrong answer"
    fi
else
    bad "multi-TU link failed (the in-class friends are not inline?)"
    sed 's/^/       /' "$WORK/odr.log" | head -12
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== the graded files build on their own ==\033[0m\n'

# The grader compiles its own main against bigint.cpp + bigint.hpp, so
# bigint.cpp must not define main, and the header must be self-sufficient.
if c++ -std=c++98 -Wall -Wextra -Werror -c "$SRC/bigint.cpp" -o "$WORK/b.o" 2>/dev/null; then
    if nm -C "$WORK/b.o" 2>/dev/null | grep -q ' T main'; then
        bad "bigint.cpp defines main() -- it would clash with the grader's main"
    else
        ok "bigint.cpp defines no main()"
    fi
else
    bad "bigint.cpp does not compile on its own"
fi

printf '#include "bigint.hpp"\n' > "$WORK/solo.cpp"
if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/solo.cpp" 2>/dev/null; then
    ok "bigint.hpp is self-contained (compiles with nothing included before it)"
else
    bad "bigint.hpp needs another header to be included first"
fi

printf '#include "bigint.hpp"\n#include "bigint.hpp"\nint main(){ return 0; }\n' > "$WORK/twice.cpp"
if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/twice.cpp" 2>/dev/null; then
    ok "bigint.hpp is safe to include twice (include guard works)"
else
    bad "including bigint.hpp twice breaks the build"
fi

# The subject's own main is the acceptance criterion; it must build with the
# exam's flags and nothing else.
if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" \
       "$SRC/bigint.cpp" "$SRC/main.cpp" -o "$WORK/subj" 2>"$WORK/subj.log"; then
    ok "the subject's main.cpp builds with -Wall -Wextra -Werror -std=c++98"
else
    bad "the subject's main.cpp does not build"
    sed 's/^/       /' "$WORK/subj.log" | head -12
fi

echo
echo "-----------------------------"
if [ $fails -eq 0 ]; then
    printf '\033[32mall compile-time checks passed\033[0m\n'
    exit 0
fi
printf '\033[1;31m%d compile-time check(s) failed\033[0m\n' "$fails"
exit 1
