#!/bin/sh
# Mutation testing: a test suite is only worth what it catches. Each mutant
# below injects one realistic bug into one of the six graded files -- a search
# that descends the wrong branch, a set that forgets to deduplicate, an
# assignment operator that loses its self-assignment guard. Every mutant must
# make the suite FAIL. A mutant that survives means the suite has a blind spot.
#
# The provided files (bag.hpp, searchable_bag.hpp, array_bag.*, tree_bag.*) are
# never mutated: the grader supplies its own copies of those.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/polyset-mutants.$$
CXX=${CXX:-c++}
FLAGS="-std=c++98 -Wall -Wextra"
SOURCES="array_bag.cpp tree_bag.cpp searchable_array_bag.cpp searchable_tree_bag.cpp set.cpp"

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

killed=0
survived=0

# describe / file to mutate / sed script
mutate() {
    name=$1
    file=$2
    edit=$3

    rm -rf "$WORK/m"
    mkdir -p "$WORK/m/tests"
    for f in bag.hpp searchable_bag.hpp array_bag.hpp array_bag.cpp tree_bag.hpp \
             tree_bag.cpp searchable_array_bag.hpp searchable_array_bag.cpp \
             searchable_tree_bag.hpp searchable_tree_bag.cpp set.hpp set.cpp; do
        cp "$SRC/$f" "$WORK/m/$f"
    done
    sed "$edit" "$SRC/$file" > "$WORK/m/$file"
    # The tests must be copied in beside the mutant: they include
    # "../searchable_array_bag.hpp" relative to their own directory, so
    # compiling them in place would silently pick up the pristine headers.
    cp "$HERE/vtest.hpp" "$HERE/test_units.cpp" "$HERE/test_fuzz.cpp" "$WORK/m/tests/"

    if cmp -s "$WORK/m/$file" "$SRC/$file"; then
        printf '  \033[1;33m[??]\033[0m %-50s mutation did not apply\n' "$name"
        survived=$((survived + 1))
        return
    fi

    objs=""
    for f in $SOURCES; do objs="$objs $WORK/m/$f"; done

    if ! $CXX $FLAGS -I"$WORK/m" $objs "$WORK/m/tests/test_units.cpp" \
            -o "$WORK/m/units" 2>"$WORK/m/build.log"; then
        printf '  \033[32m[killed]\033[0m %-48s rejected at compile time\n' "$name"
        killed=$((killed + 1))
        return
    fi
    $CXX $FLAGS -I"$WORK/m" $objs "$WORK/m/tests/test_fuzz.cpp" \
            -o "$WORK/m/fuzz" 2>/dev/null

    # Run through sh -c so a mutant that crashes reports through $? instead of
    # printing "Segmentation fault" onto this script's own output.
    units_out=$(sh -c '"$0" 2>&1' "$WORK/m/units"); units_rc=$?
    fuzz_out=$(sh -c '"$0" --bags 800 --sets 800 --views 600 2>&1' "$WORK/m/fuzz"); fuzz_rc=$?

    if [ $units_rc -eq 0 ] && [ $fuzz_rc -eq 0 ]; then
        printf '  \033[1;31m[SURVIVED]\033[0m %-46s suite has a blind spot here\n' "$name"
        survived=$((survived + 1))
        return
    fi

    if [ $units_rc -gt 128 ] || [ $fuzz_rc -gt 128 ]; then
        printf '  \033[32m[killed]\033[0m %-48s crashed under test\n' "$name"
    else
        by=""
        [ $units_rc -ne 0 ] && by="units"
        [ $fuzz_rc -ne 0 ] && by="${by:+$by+}fuzz"
        n=$(printf '%s\n' "$units_out" | grep -c '\[KO\]')
        printf '  \033[32m[killed]\033[0m %-48s by %-11s (%s unit checks failed)\n' \
               "$name" "$by" "$n"
    fi
    killed=$((killed + 1))
}

A=searchable_array_bag.hpp
T=searchable_tree_bag.hpp
S=set.hpp

echo "Mutation testing the six graded polyset files against the suite"
echo

echo "  -- searchable_array_bag --"
mutate "array has() never finds anything"        $A 's|^\t\t\t\treturn true;|\t\t\t\treturn false;|'
mutate "array has() always says yes"             $A 's|^\t\treturn false;|\t\treturn true;|'
mutate "array has() skips the first value"       $A 's|for (int i = 0; i < size; i++)|for (int i = 1; i < size; i++)|'
mutate "array has() skips the last value"        $A 's|for (int i = 0; i < size; i++)|for (int i = 0; i < size - 1; i++)|'
mutate "array has() compares the index"          $A 's|if (data\[i\] == v)|if (i == v)|'
mutate "array has() is not const"                $A 's|bool has(int v) const {|bool has(int v) {|'
mutate "array copy ctor makes an empty bag"      $A 's|searchable_array_bag(const searchable_array_bag \&o) : array_bag(o) {}|searchable_array_bag(const searchable_array_bag \&o) { (void)o; }|'
mutate "array operator= does nothing"            $A 's|{ if (this != \&o) array_bag::operator=(o); return \*this; }|{ (void)o; return *this; }|'
mutate "array operator= loses the self guard"    $A 's|{ if (this != \&o) array_bag::operator=(o); return \*this; }|{ array_bag::operator=(o); return *this; }|'
mutate "array operator= inverts the self guard"  $A 's|if (this != \&o) array_bag::operator=(o);|if (this == \&o) array_bag::operator=(o);|'
mutate "array operator= returns a copy"          $A 's|searchable_array_bag \&operator=(const searchable_array_bag \&o)|searchable_array_bag operator=(const searchable_array_bag \&o)|'

echo
echo "  -- searchable_tree_bag --"
mutate "tree has() descends the wrong way"       $T 's|n = v < n->value ? n->l : n->r|n = v < n->value ? n->r : n->l|'
mutate "tree has() only looks at the root"       $T 's|for (node \*n = tree; n; n = v < n->value ? n->l : n->r)|for (node *n = tree; n; n = 0)|'
mutate "tree has() never leaves the left spine"  $T 's|n = v < n->value ? n->l : n->r|n = n->l|'
mutate "tree has() never finds anything"         $T 's|^\t\t\t\treturn true;|\t\t\t\treturn false;|'
mutate "tree has() always says yes"              $T 's|^\t\treturn false;|\t\treturn true;|'
mutate "tree has() accepts anything not smaller" $T 's|if (n->value == v)|if (n->value >= v)|'
mutate "tree has() is not const"                 $T 's|bool has(int v) const {|bool has(int v) {|'
mutate "tree copy ctor makes an empty bag"       $T 's|searchable_tree_bag(const searchable_tree_bag \&o) : tree_bag(o) {}|searchable_tree_bag(const searchable_tree_bag \&o) { (void)o; }|'
mutate "tree operator= does nothing"             $T 's|{ if (this != \&o) tree_bag::operator=(o); return \*this; }|{ (void)o; return *this; }|'
mutate "tree operator= loses the self guard"     $T 's|{ if (this != \&o) tree_bag::operator=(o); return \*this; }|{ tree_bag::operator=(o); return *this; }|'
mutate "tree operator= inverts the self guard"   $T 's|if (this != \&o) tree_bag::operator=(o);|if (this == \&o) tree_bag::operator=(o);|'
mutate "tree operator= returns a copy"           $T 's|searchable_tree_bag \&operator=(const searchable_tree_bag \&o)|searchable_tree_bag operator=(const searchable_tree_bag \&o)|'

echo
echo "  -- set --"
mutate "set does not deduplicate"                $S 's|void insert(int v) { if (!_bag->has(v)) _bag->insert(v); }|void insert(int v) { _bag->insert(v); }|'
mutate "set inserts only duplicates"             $S 's|if (!_bag->has(v)) _bag->insert(v);|if (_bag->has(v)) _bag->insert(v);|'
mutate "set never inserts anything"              $S 's|void insert(int v) { if (!_bag->has(v)) _bag->insert(v); }|void insert(int v) { (void)v; }|'
mutate "set bulk insert is off by one"           $S 's|for (int i = 0; i < n; i++) insert(arr\[i\]);|for (int i = 0; i < n - 1; i++) insert(arr[i]);|'
mutate "set bulk insert skips the dedup"         $S 's|for (int i = 0; i < n; i++) insert(arr\[i\]);|for (int i = 0; i < n; i++) _bag->insert(arr[i]);|'
mutate "set bulk insert repeats the first value" $S 's|for (int i = 0; i < n; i++) insert(arr\[i\]);|for (int i = 0; i < n; i++) insert(arr[0]);|'
mutate "set has() is inverted"                   $S 's|bool has(int v) const { return _bag->has(v); }|bool has(int v) const { return !_bag->has(v); }|'
mutate "set print() prints nothing"              $S 's|void print() const { _bag->print(); }|void print() const {}|'
mutate "set clear() does nothing"                $S 's|void clear() { _bag->clear(); }|void clear() {}|'
mutate "set copy ctor drops the bag"             $S 's|set(const set \&o) : _bag(o._bag) {}|set(const set \&o) : _bag(0) { (void)o; }|'
mutate "set operator= does not rebind"           $S 's|set \&operator=(const set \&o) { _bag = o._bag; return \*this; }|set \&operator=(const set \&o) { (void)o; return *this; }|'
mutate "set operator= returns a copy"            $S 's|set \&operator=(const set \&o)|set operator=(const set \&o)|'
mutate "set has() is not const"                  $S 's|bool has(int v) const { return _bag->has(v); }|bool has(int v) { return _bag->has(v); }|'
mutate "set print() is not const"                $S 's|void print() const { _bag->print(); }|void print() { _bag->print(); }|'
mutate "get_bag() is not const"                  $S 's|searchable_bag \&get_bag() const { return \*_bag; }|searchable_bag \&get_bag() { return *_bag; }|'

echo
echo "-----------------------------"
if [ $survived -eq 0 ]; then
    printf '\033[32m%d/%d mutants killed\033[0m\n' "$killed" "$((killed + survived))"
    exit 0
fi
printf '\033[1;31m%d/%d mutants killed, %d survived\033[0m\n' \
       "$killed" "$((killed + survived))" "$survived"
exit 1
