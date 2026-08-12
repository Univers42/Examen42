#!/bin/sh
# Mutation testing: a test suite is only worth what it catches. Each mutant
# below injects one realistic vect2 bug (most of them are mistakes that were
# actually made in the training versions of this exercise). Every mutant must
# make the suite FAIL. A mutant that survives means the suite has a blind spot.
#
# Each mutant is one sed expression, applied to both vect2.cpp and vect2.hpp.
# Whichever file holds the implementation is the one that changes, so the same
# mutant list works for a header-only vect2 and for one split across the pair.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/vect2-mutants.$$
CXX=${CXX:-c++}
FLAGS="-std=c++98 -Wall -Wextra"

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

killed=0
survived=0
noapply=0

# Only live code counts. A sed that rewrites a commented-out definition changes
# the file without changing the program, and would otherwise be reported as a
# genuine survivor instead of a mutant that never applied.
live_code() {
    sed -e 's|//.*||' -e '/^[[:space:]]*$/d' "$1"
}

changed() { # pristine mutant
    live_code "$1" > "$WORK/live.a"
    live_code "$2" > "$WORK/live.b"
    ! cmp -s "$WORK/live.a" "$WORK/live.b"
}

# describe / sed script
mutate() {
    name=$1
    edit=$2

    rm -rf "$WORK/m"
    mkdir -p "$WORK/m/tests"
    sed "$edit" "$SRC/vect2.cpp" > "$WORK/m/vect2.cpp"
    sed "$edit" "$SRC/vect2.hpp" > "$WORK/m/vect2.hpp"
    # The tests must be copied in beside the mutant, because they include
    # "../vect2.hpp" relative to their own directory -- compiling them in place
    # would silently pick up the pristine header and miss every mutation that
    # lives in it (operator<< and the free n * v).
    cp "$HERE/vtest.hpp" "$HERE/test_units.cpp" "$HERE/test_fuzz.cpp" "$WORK/m/tests/"

    if changed "$SRC/vect2.cpp" "$WORK/m/vect2.cpp" || \
       changed "$SRC/vect2.hpp" "$WORK/m/vect2.hpp"; then
        :
    else
        printf '  \033[1;33m[??]\033[0m %-44s mutation did not apply\n' "$name"
        noapply=$((noapply + 1))
        return
    fi

    if ! $CXX $FLAGS "$WORK/m/vect2.cpp" "$WORK/m/tests/test_units.cpp" \
            -o "$WORK/m/units" 2>"$WORK/m/build.log"; then
        printf '  \033[32m[killed]\033[0m %-42s rejected at compile time\n' "$name"
        killed=$((killed + 1))
        return
    fi
    $CXX $FLAGS "$WORK/m/vect2.cpp" "$WORK/m/tests/test_fuzz.cpp" \
            -o "$WORK/m/fuzz" 2>/dev/null

    # Run through sh -c so a mutant that crashes reports through $? instead of
    # printing "Segmentation fault" onto this script's own output.
    units_out=$(sh -c '"$0" 2>&1' "$WORK/m/units"); units_rc=$?
    fuzz_out=$(sh -c '"$0" --steps 20000 --trees 3000 2>&1' "$WORK/m/fuzz"); fuzz_rc=$?

    if [ $units_rc -eq 0 ] && [ $fuzz_rc -eq 0 ]; then
        printf '  \033[1;31m[SURVIVED]\033[0m %-40s suite has a blind spot here\n' "$name"
        survived=$((survived + 1))
        return
    fi

    if [ $units_rc -gt 128 ] || [ $fuzz_rc -gt 128 ]; then
        printf '  \033[32m[killed]\033[0m %-42s crashed under test\n' "$name"
    else
        by=""
        [ $units_rc -ne 0 ] && by="units"
        [ $fuzz_rc -ne 0 ] && by="${by:+$by+}fuzz"
        n=$(printf '%s\n' "$units_out" | grep -c '\[KO\]')
        printf '  \033[32m[killed]\033[0m %-42s by %-11s (%s unit checks failed)\n' \
               "$name" "$by" "$n"
    fi
    killed=$((killed + 1))
}

echo "Mutation testing vect2 against the suite"
echo

#      name                                  sed script
mutate "operator+ swaps x and y"             's|_x + other._x, _y + other._y|_y + other._y, _x + other._x|'
mutate "operator+ mutates its operand"       's|vect2 operator+(const vect2& other) const .*|vect2 operator+(const vect2\& other) const {vect2 t(_x + other._x, _y + other._y); const_cast<vect2*>(this)->_x = t._x; return (t);}|'
mutate "operator- is really addition"        's|_x - other._x, _y - other._y|_x + other._x, _y + other._y|'
mutate "operator* forgets y"                 's|vect2(_x \* n, _y \* n)|vect2(_x * n, _y)|'
mutate "unary - only negates x"              's|vect2(-_x, -_y)|vect2(-_x, _y)|'
mutate "unary - is not const"                's|vect2 operator-(void) const|vect2 operator-(void)|'
mutate "postfix ++ returns the new value"    's|vect2 operator++(int) .*|vect2 operator++(int) {++(*this); return (*this);}|'
mutate "postfix -- returns the new value"    's|vect2 operator--(int) .*|vect2 operator--(int) {--(*this); return (*this);}|'
mutate "prefix ++ steps only x"              's|((++_x, ++_y), \*this)|((++_x), *this)|'
mutate "prefix -- steps only y"              's|((--_x, --_y), \*this)|((--_y), *this)|'
mutate "operator-= is really +="             's|_x -= other\[0\]; _y -= other\[1\]|_x += other[0]; _y += other[1]|'
mutate "operator*= forgets to return"        's|vect2& operator\*=(const int n){.*|vect2 operator*=(const int n){_x *= n; _y *= n; return (vect2(_x, _y));}|'
mutate "operator+= returns a copy"           's|vect2& operator+=(const vect2& other)|vect2 operator+=(const vect2\& other)|'
mutate "operator= returns a copy"            's|vect2& operator=(const vect2& other)|vect2 operator=(const vect2\& other)|'
mutate "operator= skips equal values"        's|{_x = other._x; _y = other._y; return (\*this);}|{if (*this == other) {_x = other._x; _y = other._y;} return (*this);}|'
mutate "operator\[\] swaps 0 and 1"          's|idx == 0 ? _x : _y|idx == 0 ? _y : _x|'
mutate "const operator\[\] always gives x"   's|int operator\[\](int idx) const .*|int operator[](int idx) const {(void)idx; return (_x);}|'
mutate "operator\[\] returns a dangling copy" 's|int& operator\[\](int idx) {.*|int\& operator[](int idx) {static int t; t = (idx == 0 ? _x : _y); return (t);}|'
mutate "operator== only compares x"          's|((_x == o._x) && (_y == o._y))|(_x == o._x)|'
mutate "operator== is not const"             's|bool operator==(const vect2& o) const|bool operator==(const vect2\& o)|'
mutate "operator!= is a copy of =="          's|return (!(\*this == o));|return (*this == o);|'
mutate "operator<< uses parentheses"         's|"{" << other\[0\] << ", " << other\[1\] << "}"|"(" << other[0] << ", " << other[1] << ")"|'
mutate "operator<< drops the separator"      's|", " << other\[1\]|"," << other[1]|'
mutate "operator<< swaps the components"     's|other\[0\] << ", " << other\[1\]|other[1] << ", " << other[0]|'
mutate "free n * v swaps components"         's|friend vect2 operator\*(const int n, const vect2& o){.*|friend vect2 operator*(const int n, const vect2\& o){return (vect2(o[1] * n, o[0] * n));}|'
mutate "free n * v ignores the scalar"       's|friend vect2 operator\*(const int n, const vect2& o){.*|friend vect2 operator*(const int n, const vect2\& o){(void)n; return (vect2(o[0], o[1]));}|'
mutate "default ctor leaves x at 1"          's|vect2(): _x(0), _y(0)|vect2(): _x(1), _y(0)|'
mutate "copy ctor drops y"                   's|_x(other._x), _y(other._y)|_x(other._x), _y(0)|'

echo
echo "-----------------------------"
total=$((killed + survived + noapply))
if [ $survived -eq 0 ] && [ $noapply -eq 0 ]; then
    printf '\033[32m%d/%d mutants killed\033[0m\n' "$killed" "$total"
    exit 0
fi
printf '\033[1;31m%d/%d mutants killed' "$killed" "$total"
[ $survived -ne 0 ] && printf ', %d survived' "$survived"
[ $noapply -ne 0 ] && printf ', %d never applied' "$noapply"
printf '\033[0m\n'
[ $noapply -ne 0 ] && printf 'A mutant that never applied means its sed no longer matches the\nimplementation -- retarget it, it is not testing anything right now.\n'
exit 1
