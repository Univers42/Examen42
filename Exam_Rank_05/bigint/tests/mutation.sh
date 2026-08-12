#!/bin/sh
# Mutation testing: a test suite is only worth what it catches. Each mutant
# below injects one realistic bigint bug -- a dropped carry, a comparison that
# forgets that width beats lexicographic order, a shift that normalises zero
# into "0000". Every mutant must make the suite FAIL. A mutant that survives
# means the suite has a blind spot.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/bigint-mutants.$$
CXX=${CXX:-c++}
FLAGS="-std=c++98 -Wall -Wextra"

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

killed=0
survived=0

# describe / sed-script-on-cpp / sed-script-on-hpp
mutate() {
    name=$1
    cpp_edit=$2
    hpp_edit=$3

    rm -rf "$WORK/m"
    mkdir -p "$WORK/m/tests"
    sed "$cpp_edit" "$SRC/bigint.cpp" > "$WORK/m/bigint.cpp"
    sed "$hpp_edit" "$SRC/bigint.hpp" > "$WORK/m/bigint.hpp"
    # The tests must be copied in beside the mutant, because they include
    # "../bigint.hpp" relative to their own directory -- compiling them in
    # place would silently pick up the pristine header and miss every mutation
    # that lives in it (which is most of them: the class is header-heavy).
    cp "$HERE/vtest.hpp" "$HERE/test_units.cpp" "$HERE/test_fuzz.cpp" "$WORK/m/tests/"

    if cmp -s "$WORK/m/bigint.cpp" "$SRC/bigint.cpp" && \
       cmp -s "$WORK/m/bigint.hpp" "$SRC/bigint.hpp"; then
        printf '  \033[1;33m[??]\033[0m %-46s mutation did not apply\n' "$name"
        survived=$((survived + 1))
        return
    fi

    if ! $CXX $FLAGS "$WORK/m/bigint.cpp" "$WORK/m/tests/test_units.cpp" \
            -o "$WORK/m/units" 2>"$WORK/m/build.log"; then
        printf '  \033[32m[killed]\033[0m %-44s rejected at compile time\n' "$name"
        killed=$((killed + 1))
        return
    fi
    $CXX $FLAGS "$WORK/m/bigint.cpp" "$WORK/m/tests/test_fuzz.cpp" \
            -o "$WORK/m/fuzz" 2>/dev/null

    # Run through sh -c so a mutant that crashes or throws reports through $?
    # instead of printing onto this script's own output.
    units_out=$(sh -c '"$0" 2>&1' "$WORK/m/units"); units_rc=$?
    fuzz_out=$(sh -c '"$0" --steps 20000 --exprs 3000 2>&1' "$WORK/m/fuzz"); fuzz_rc=$?

    if [ $units_rc -eq 0 ] && [ $fuzz_rc -eq 0 ]; then
        printf '  \033[1;31m[SURVIVED]\033[0m %-42s suite has a blind spot here\n' "$name"
        survived=$((survived + 1))
        return
    fi

    if [ $units_rc -gt 128 ] || [ $fuzz_rc -gt 128 ]; then
        printf '  \033[32m[killed]\033[0m %-44s crashed under test\n' "$name"
    else
        by=""
        [ $units_rc -ne 0 ] && by="units"
        [ $fuzz_rc -ne 0 ] && by="${by:+$by+}fuzz"
        n=$(printf '%s\n' "$units_out" | grep -c '\[KO\]')
        printf '  \033[32m[killed]\033[0m %-44s by %-11s (%s unit checks failed)\n' \
               "$name" "$by" "$n"
    fi
    killed=$((killed + 1))
}

echo "Mutation testing bigint against the suite"
echo

# ---- construction and canonical form (all in the header) -------------------
mutate "default ctor is the empty string"   '' 's|bigint() : s("0") {}|bigint() : s("") {}|'
mutate "bigint(int) loses the last digit"   '' 's|do { s.insert(s.begin(), char(.0. + n % 10)); n /= 10; } while (n);|while (n) { s.insert(s.begin(), char(0x30 + n % 10)); n /= 10; }|'
mutate "bigint(int) reverses the digits"    '' 's|s.insert(s.begin(), char(.0. + n % 10));|s += char(0x30 + n % 10);|'
mutate "bigint(int) forgets to divide"      '' 's|n /= 10; } while (n);|n /= 100; } while (n);|'
mutate "string ctor keeps leading zeros"    '' 's|: s(v.find_first_not_of(.0.) == std::string::npos ? "0" : v.substr(v.find_first_not_of(.0.)))|: s(v.empty() ? "0" : v)|'
mutate "string ctor eats a real digit"      '' 's|v.substr(v.find_first_not_of(.0.))|v.substr(v.find_first_not_of(0x30) + 1)|'
mutate "copy ctor produces zero"            '' 's|bigint(const bigint\& o) : s(o.s) {}|bigint(const bigint\& o) : s("0") {}|'
mutate "operator= does not assign"          '' 's|{ s = o.s; return \*this; }|{ return *this; }|'
mutate "operator= returns a copy"           '' 's|bigint\& operator=(const bigint\& o)|bigint operator=(const bigint\& o)|'

# ---- addition (the interesting half lives in the .cpp) ---------------------
mutate "+= drops the carry out"             's|this->s.insert(this->s.begin(), .1.);|(void)0;|' ''
mutate "+= drops the carry in"              's|int d = (\*sq - .0.) + (\*bq - .0.) + carry;|int d = (*sq - 0x30) + (*bq - 0x30);|' ''
mutate "+= carries one digit too late"      's|carry = (d >= 10);|carry = (d > 10);|' ''
mutate "+= forgets to subtract the carry"   's|\*sq = char(.0. + d - 10 \* carry);|*sq = char(0x30 + d);|' ''
mutate "+= grafts one digit too few"        's|this->s.insert(0, b, 0, nb - lo);|this->s.insert(0, b, 0, nb - lo - 1);|' ''
mutate "+= reads past the short operand"    's|^\t\tlo = nb;|\t\tlo = this->s.size();|' ''
mutate "+= ripple stops one digit early"    's|for (; carry \&\& i < n; ++i, --sq)|for (; carry \&\& i + 1 < n; ++i, --sq)|' ''
mutate "+= ripple mishandles a 9"           's|if (\*sq == .9.)|if (*sq == 0x38)|' ''
mutate "+= ripple forgets to clear carry"   's|^\t\t\tcarry = 0;|\t\t\t(void)0;|' ''
mutate "+= returns a copy"                  's|^bigint\& bigint::operator+=|bigint bigint::operator+=|' 's|bigint\& operator+=(const bigint\& o);|bigint operator+=(const bigint\& o);|'

# ---- increment -------------------------------------------------------------
mutate "prefix ++ steps by two"             '' 's|bigint\& operator++() { return \*this += 1; }|bigint\& operator++() { return *this += 2; }|'
mutate "prefix ++ returns a copy"           '' 's|bigint\& operator++() { return \*this += 1; }|bigint operator++() { bigint t(*this); *this += 1; return t; }|'
mutate "postfix ++ returns the new value"   '' 's|bigint operator++(int) { bigint t(\*this); \*this += 1; return t; }|bigint operator++(int) { *this += 1; return *this; }|'
mutate "postfix ++ does not increment"      '' 's|bigint operator++(int) { bigint t(\*this); \*this += 1; return t; }|bigint operator++(int) { bigint t(*this); return t; }|'

# ---- digitshift ------------------------------------------------------------
mutate "<<= denormalises zero"              's|if (this->s != "0")|if (1)|' ''
mutate "<<= appends the wrong digit"        's|this->s.append(count(k.s), .0.);|this->s.append(count(k.s), 0x31);|' ''
mutate "<<= is off by one"                  's|this->s.append(count(k.s), .0.);|this->s.append(count(k.s) + 1, 0x30);|' ''
mutate ">>= leaves an empty string"         's|if (n >= this->s.size())|if (n > this->s.size())|' ''
mutate ">>= erases from the front"          's|s.erase(s.size() - n);|s.erase(0, n);|' ''
mutate ">>= is off by one"                  's|s.erase(s.size() - n);|s.erase(s.size() - n - 1);|' ''
mutate "count sums the digits"              's|k = k \* 10 + size_t(s\[i\] - .0.);|k = k + size_t(s[i] - 0x30);|' ''
mutate "free << shifts the wrong way"       '' 's|friend bigint operator<<(bigint a, const bigint\& k) { return a <<= k; }|friend bigint operator<<(bigint a, const bigint\& k) { return a >>= k; }|'
mutate "free >> mutates its operand"        '' 's|friend bigint operator>>(bigint a, const bigint\& k) { return a >>= k; }|friend bigint operator>>(bigint\& a, const bigint\& k) { return a >>= k; }|'
mutate "free + mutates its operand"         '' 's|friend bigint operator+(bigint a, const bigint\& b) { return a += b; }|friend bigint operator+(bigint\& a, const bigint\& b) { return a += b; }|'
mutate "free + doubles the left operand"    '' 's|friend bigint operator+(bigint a, const bigint\& b) { return a += b; }|friend bigint operator+(bigint a, const bigint\& b) { (void)b; return a += a; }|'

# ---- comparison ------------------------------------------------------------
mutate "< ignores the width rule"           '' 's|{ return a.s.size() != b.s.size() ? a.s.size() < b.s.size() : a.s < b.s; }|{ return a.s < b.s; }|'
mutate "< has the width rule backwards"     '' 's|? a.s.size() < b.s.size() :|? a.s.size() > b.s.size() :|'
mutate "< is really <="                     '' 's|: a.s < b.s; }|: a.s <= b.s; }|'
mutate "== compares widths only"            '' 's|friend bool operator==(const bigint\& a, const bigint\& b) { return a.s == b.s; }|friend bool operator==(const bigint\& a, const bigint\& b) { return a.s.size() == b.s.size(); }|'
mutate "> is really <"                      '' 's|friend bool operator>(const bigint\& a, const bigint\& b) { return b < a; }|friend bool operator>(const bigint\& a, const bigint\& b) { return a < b; }|'
mutate "<= drops the negation"              '' 's|friend bool operator<=(const bigint\& a, const bigint\& b) { return !(b < a); }|friend bool operator<=(const bigint\& a, const bigint\& b) { return (b < a); }|'
mutate ">= uses the wrong operand order"    '' 's|friend bool operator>=(const bigint\& a, const bigint\& b) { return !(a < b); }|friend bool operator>=(const bigint\& a, const bigint\& b) { return !(b < a); }|'
mutate "!= is a copy of =="                 '' 's|friend bool operator!=(const bigint\& a, const bigint\& b) { return !(a == b); }|friend bool operator!=(const bigint\& a, const bigint\& b) { return (a == b); }|'

# ---- output ----------------------------------------------------------------
mutate "<< pads with a leading zero"        '' 's|{ return os << b.s; }|{ return os << "0" << b.s; }|'
mutate "<< does not return the stream"      '' 's|friend std::ostream\& operator<<(std::ostream\& os, const bigint\& b) { return os << b.s; }|friend std::ostream\& operator<<(std::ostream\& os, const bigint\& b) { os << b.s; return std::cout; }|'

echo
echo "-----------------------------"
if [ $survived -eq 0 ]; then
    printf '\033[32m%d/%d mutants killed\033[0m\n' "$killed" "$((killed + survived))"
    exit 0
fi
printf '\033[1;31m%d/%d mutants killed, %d survived\033[0m\n' \
       "$killed" "$((killed + survived))" "$survived"
exit 1
