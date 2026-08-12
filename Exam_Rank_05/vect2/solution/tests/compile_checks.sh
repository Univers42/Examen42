#!/bin/sh
# Checks that can only be made at compile time:
#   1. the warning matrix (several compilers x several standards)
#   2. const-correctness, expressed as "this snippet must NOT compile"
#   3. ODR: the header can be included by more than one translation unit
#
# The header defines operator<< and the free  n * v  inside the class body, so
# they are implicitly inline -- check 3 is what proves that is really the case.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/vect2-compile.$$

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

fails=0
ok()   { printf '  \033[32m[ok]\033[0m %s\n' "$1"; }
bad()  { printf '  \033[1;31m[KO]\033[0m %s\n' "$1"; fails=$((fails + 1)); }

# --------------------------------------------------------------------------
printf '\n\033[36m== Warning matrix ==\033[0m\n'

# -Wall -Wextra -Werror is what the exam uses; the rest is extra pressure.
STRICT="-Wall -Wextra -Werror -pedantic-errors -Wshadow -Wold-style-cast -Wconversion -Wsign-conversion -Wcast-qual -Wunused -Weffc++"

for cxx in g++ clang++; do
    command -v $cxx >/dev/null 2>&1 || { printf '  \033[1;33m[--]\033[0m %s not installed\n' "$cxx"; continue; }
    for std in c++98 c++03 c++11 c++17; do
        log=$($cxx -std=$std $STRICT -c "$SRC/vect2.cpp" -o /dev/null 2>&1)
        if [ -z "$log" ]; then
            ok "$cxx -std=$std  (exam flags + shadow/conversion/effc++)"
        else
            bad "$cxx -std=$std"
            printf '%s\n' "$log" | sed 's/^/       /' | head -12
        fi
    done
done

# --------------------------------------------------------------------------
printf '\n\033[36m== const-correctness (must be rejected) ==\033[0m\n'

reject() {
    printf '#include "vect2.hpp"\nint main(){ %s ; return 0; }\n' "$1" > "$WORK/t.cpp"
    if c++ -std=c++98 -fsyntax-only -I"$SRC" "$WORK/t.cpp" 2>/dev/null; then
        bad "compiles but should not:  $1"
    else
        ok "rejected:  $1"
    fi
}

accept() {
    printf '#include "vect2.hpp"\nint main(){ %s ; return 0; }\n' "$1" > "$WORK/t.cpp"
    if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/t.cpp" 2>"$WORK/e.log"; then
        ok "accepted:  $1"
    else
        bad "should compile but does not:  $1"
        sed 's/^/       /' "$WORK/e.log" | head -8
    fi
}

reject 'const vect2 c(1,2); c[0] = 5;'
reject 'const vect2 c(1,2); ++c;'
reject 'const vect2 c(1,2); c++;'
reject 'const vect2 c(1,2); --c;'
reject 'const vect2 c(1,2); c--;'
reject 'const vect2 c(1,2); c += vect2(1,1);'
reject 'const vect2 c(1,2); c -= vect2(1,1);'
reject 'const vect2 c(1,2); c *= 2;'
reject 'const vect2 c(1,2); c = vect2(1,1);'
reject 'vect2 v(1,2); int *p = &v[0]; *p = 1; v.x = 3;'          # x is private

printf '\n\033[36m== every operation a const vect2 must still support ==\033[0m\n'
accept 'const vect2 c(1,2); (void)c[0]; (void)c[1];'
accept 'const vect2 c(1,2); (void)(c + c); (void)(c - c);'
accept 'const vect2 c(1,2); (void)(c * 2); (void)(2 * c); (void)(-c);'
accept 'const vect2 c(1,2); (void)(c == c); (void)(c != c);'
accept 'const vect2 c(1,2); std::cout << c;'
accept 'const vect2 c(1,2); vect2 copy(c); vect2 a; a = c; (void)copy;'
accept 'const vect2 c(1,2); (void)(c + c * 2);'
accept 'vect2 a(2,2); (void)(a * 2 == vect2(4,4));'
accept 'vect2 v; v[0] = 1; v[1] = 2; int& r = v[0]; r = 3; (void)r;'

# --------------------------------------------------------------------------
printf '\n\033[36m== ODR: header included from two translation units ==\033[0m\n'

cat > "$WORK/tu_a.cpp" <<'EOF'
#include "vect2.hpp"
vect2 buildA() { return (vect2(1, 2) + 3 * vect2(1, 1)); }
std::string renderA(const vect2& v) { std::ostringstream os; os << v; return (os.str()); }
EOF

cat > "$WORK/tu_b.cpp" <<'EOF'
#include "vect2.hpp"
#include <cstdlib>
vect2 buildA();
std::string renderA(const vect2& v);
int main() {
    vect2 local = 2 * vect2(2, 2) - vect2(0, 1);
    std::ostringstream os;
    os << local;
    if (!(buildA() == vect2(4, 5)))     return (1);
    if (renderA(local) != "{4, 3}")     return (2);
    if (os.str() != renderA(local))     return (3);
    std::cout << "  both TUs agree: " << buildA() << " and " << local << "\n";
    return (0);
}
EOF

# sstream/string are pulled in by the test units, not by vect2.hpp itself.
sed -i '1i #include <sstream>\n#include <string>' "$WORK/tu_a.cpp" "$WORK/tu_b.cpp"

if c++ -std=c++98 -Wall -Wextra -Werror -I"$SRC" \
       "$SRC/vect2.cpp" "$WORK/tu_a.cpp" "$WORK/tu_b.cpp" -o "$WORK/odr" 2>"$WORK/odr.log"; then
    if "$WORK/odr"; then
        ok "links and runs with the header in 3 translation units"
    else
        bad "multi-TU program ran but produced the wrong answer"
    fi
else
    bad "multi-TU link failed (operator<< / operator* not inline?)"
    sed 's/^/       /' "$WORK/odr.log" | head -12
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== the graded files build on their own ==\033[0m\n'

# The grader compiles its own main against vect2.cpp + vect2.hpp, so vect2.cpp
# must not define main, and the header must be self-sufficient.
if c++ -std=c++98 -Wall -Wextra -Werror -c "$SRC/vect2.cpp" -o "$WORK/v.o" 2>/dev/null; then
    if nm -C "$WORK/v.o" 2>/dev/null | grep -q ' T main'; then
        bad "vect2.cpp defines main() -- it would clash with the grader's main"
    else
        ok "vect2.cpp defines no main()"
    fi
else
    bad "vect2.cpp does not compile on its own"
fi

printf '#include "vect2.hpp"\n' > "$WORK/solo.cpp"
if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/solo.cpp" 2>/dev/null; then
    ok "vect2.hpp is self-contained (compiles with nothing included before it)"
else
    bad "vect2.hpp needs another header to be included first"
fi

printf '#include "vect2.hpp"\n#include "vect2.hpp"\nint main(){ return 0; }\n' > "$WORK/twice.cpp"
if c++ -std=c++98 -Wall -Wextra -Werror -fsyntax-only -I"$SRC" "$WORK/twice.cpp" 2>/dev/null; then
    ok "vect2.hpp is safe to include twice (include guard works)"
else
    bad "including vect2.hpp twice breaks the build"
fi

echo
echo "-----------------------------"
if [ $fails -eq 0 ]; then
    printf '\033[32mall compile-time checks passed\033[0m\n'
    exit 0
fi
printf '\033[1;31m%d compile-time check(s) failed\033[0m\n' "$fails"
exit 1
