#!/bin/sh
# Mutation testing: a test suite is only worth what it catches. Each mutant
# below injects one realistic life bug -- a pen that draws on the wrong side of
# the move, a neighbour count that includes the cell itself, a survival rule
# off by one, a print loop that breaks the lines in the wrong place. Every
# mutant must make the suite FAIL. A mutant that survives means the suite has
# a blind spot.
#
# Kill channels, tried in order: the compiler, then the unit cases, then the
# fuzzer, then valgrind. The valgrind pass runs only for mutants the first
# three let through, which is what makes the memory mutants killable without
# paying for valgrind on every one.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/life-mutants.$$
CC=${CC:-cc}
FLAGS="-Wall -Wextra"

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

killed=0
survived=0

# describe / sed script on life.c
mutate() {
    name=$1
    edit=$2

    rm -rf "$WORK/m"
    mkdir -p "$WORK/m"
    cp "$SRC/life.h" "$WORK/m/life.h"
    sed "$edit" "$SRC/life.c" > "$WORK/m/life.c"

    if cmp -s "$WORK/m/life.c" "$SRC/life.c"; then
        printf '  \033[1;33m[??]\033[0m %-50s mutation did not apply\n' "$name"
        survived=$((survived + 1))
        return
    fi

    if ! $CC $FLAGS -I"$WORK/m" "$WORK/m/life.c" -o "$WORK/m/life" 2>"$WORK/m/build.log"; then
        printf '  \033[32m[killed]\033[0m %-48s rejected at compile time\n' "$name"
        killed=$((killed + 1))
        return
    fi

    units_out=$(cd "$HERE" && sh -c 'python3 test_units.py "$0" 2>&1' "$WORK/m/life"); units_rc=$?
    fuzz_out=$(cd "$HERE" && sh -c 'python3 test_fuzz.py "$0" --cases 600 --long 10 2>&1' "$WORK/m/life"); fuzz_rc=$?

    if [ $units_rc -ne 0 ] || [ $fuzz_rc -ne 0 ]; then
        by=""
        [ $units_rc -ne 0 ] && by="units"
        [ $fuzz_rc -ne 0 ] && by="${by:+$by+}fuzz"
        n=$(printf '%s\n' "$units_out" | grep -c '\[KO\]')
        printf '  \033[32m[killed]\033[0m %-48s by %-11s (%s unit checks failed)\n' \
               "$name" "$by" "$n"
        killed=$((killed + 1))
        return
    fi

    # Nothing observable changed, so the mutation is either dead code or a
    # memory bug. valgrind decides.
    ok=1
    for run in "5 5 0|sdxddssaaww" "10 6 3|sdxssdswdxddddsxaadwxwdxwaa" "3 3 2|dxss" "1 1 1|x"; do
        args=${run%%|*}
        cmds=${run##*|}
        printf '%s\n' "$cmds" | valgrind -q --error-exitcode=1 --leak-check=full \
            --errors-for-leak-kinds=all "$WORK/m/life" $args >/dev/null 2>>"$WORK/m/vg.log" || ok=0
    done
    if [ $ok -eq 0 ]; then
        printf '  \033[32m[killed]\033[0m %-48s by valgrind\n' "$name"
        killed=$((killed + 1))
        return
    fi

    # The calloc guards are only observable when calloc actually fails, so
    # squeeze the address space and ask for a board that will not fit.
    (ulimit -v 65536 2>/dev/null; printf 'x\n' | "$WORK/m/life" 20000 20000 1) >/dev/null 2>&1
    rc=$?
    if [ $rc -gt 128 ]; then
        printf '  \033[32m[killed]\033[0m %-48s crashed when calloc failed\n' "$name"
        killed=$((killed + 1))
        return
    fi

    printf '  \033[1;31m[SURVIVED]\033[0m %-46s suite has a blind spot here\n' "$name"
    survived=$((survived + 1))
}

echo "Mutation testing life against the suite"
echo

echo "  -- the pen --"
mutate "the pen never draws"                  's|b.cells\[y \* b.w + x\] = 1;|(void)0;|'
mutate "the pen always draws"                 's|if (p \&\& x >= 0|if (1 \&\& x >= 0|'
mutate "x moves instead of toggling"          's|if (c == .x.)\n||; s|^\t\t\tp = !p;$|\t\t\tx++;|'
mutate "the pen starts down"                  's|int\t\tx = 0, y = 0, p = 0, i, k, n;|int\t\tx = 0, y = 0, p = 1, i, k, n;|'
mutate "the pen starts in the wrong corner"   's|int\t\tx = 0, y = 0, p = 0|int\t\tx = 1, y = 0, p = 0|'
mutate "w and s are swapped"                  's|y += (c == .s.) - (c == .w.);|y += (c == 0x77) - (c == 0x73);|'
mutate "a and d are swapped"                  's|x += (c == .d.) - (c == .a.);|x += (c == 0x61) - (c == 0x64);|'
mutate "w and a move the same axis"           's|y += (c == .s.) - (c == .w.);|y += (c == 0x73);|'
mutate "the pen cannot move up"               's|- (c == .w.)||'
mutate "the pen cannot move left"             's| - (c == .a.)||'
mutate "a cell left of the board is drawn"    's|if (p \&\& x >= 0 \&\& x < b.w|if (p \&\& x >= -1 \&\& x < b.w|'
mutate "the board wraps horizontally"         's|if (p \&\& x >= 0 \&\& x < b.w \&\& y >= 0 \&\& y < b.h)|if (p \&\& y >= 0 \&\& y < b.h)|; s|b.cells\[y \* b.w + x\] = 1;|b.cells[y * b.w + ((x % b.w) + b.w) % b.w] = 1;|'
mutate "the row and column are swapped"       's|b.cells\[y \* b.w + x\] = 1;|b.cells[x * b.w + y] = 1;|'
mutate "the bottom row cannot be drawn"       's|y >= 0 \&\& y < b.h|y >= 0 \&\& y < b.h - 1|'
mutate "the last command is ignored"          's|while (read(0, \&c, 1) > 0)|while (read(0, \&c, 1) > 1)|'

echo
echo "  -- the neighbour count --"
mutate "the cell counts itself"               's|if ((i \|\| j) \&\&|if (1 \&\&|'
mutate "only four neighbours are counted"     's|if ((i \|\| j) \&\&|if ((!i != !j) \&\&|'
mutate "cells off the left edge are counted"  's|x + j >= 0 \&\& x + j < b->w|x + j < b->w|'
mutate "cells off the top are counted"        's|y + i >= 0 \&\& y + i < b->h|y + i < b->h|'
mutate "cells off the right edge are counted" 's|x + j >= 0 \&\& x + j < b->w \&\&|x + j >= 0 \&\&|'
mutate "the neighbourhood is one row short"   's|for (i = -1; i < 2; i++)|for (i = -1; i < 1; i++)|'
mutate "the neighbourhood is one column short" 's|for (j = -1; j < 2; j++)|for (j = 0; j < 2; j++)|'
mutate "the neighbour index is transposed"    's|c += b->cells\[(y + i) \* b->w + x + j\];|c += b->cells[(x + j) * b->w + y + i];|'

echo
echo "  -- the rule --"
mutate "a cell needs three neighbours to live" 's|(n == 3 \|\| (n == 2 \&\& b.cells\[i\]))|(n == 3)|'
mutate "two neighbours are enough to be born"  's|(n == 3 \|\| (n == 2 \&\& b.cells\[i\]))|(n == 3 \|\| n == 2)|'
mutate "four neighbours keep a cell alive"     's|(n == 2 \&\& b.cells\[i\])|(n == 2 \|\| n == 4) \&\& b.cells[i]|'
mutate "birth needs four neighbours"           's|n == 3 \|\||n == 4 \|\||'
mutate "survival needs three"                  's|(n == 2 \&\& b.cells\[i\])|(n == 3 \&\& b.cells[i])|'
mutate "the board is read while it is written" 's|t.cells\[i\] = (char)(n == 3|b.cells[i] = (char)(n == 3|'
mutate "the coordinates handed to nb are swapped" 's|nb(\&b, i % b.w, i / b.w)|nb(\&b, i / b.w, i % b.w)|'
mutate "one iteration too many"                's|while (k-- > 0)|while (k-- >= 0)|'
mutate "one iteration too few"                 's|while (k-- > 0)|while (--k > 0)|'
mutate "the new board is never adopted"        's|b.cells = t.cells;|free(t.cells);|'

echo
echo "  -- printing --"
mutate "live and dead are swapped"            's|b.cells\[i\] ? .O. : . .|b.cells[i] ? 0x20 : 0x4f|'
mutate "dead cells print as a dot"            's|? .O. : . .|? 0x4f : 0x2e|'
mutate "the line break comes one cell early"  's|if (i % b.w == b.w - 1)|if (i % b.w == b.w - 2)|'
mutate "the line break uses the height"       's|if (i % b.w == b.w - 1)|if (i % b.h == b.h - 1)|'
mutate "there are no line breaks at all"      's|^\t\t\tputchar(.\\n.);$|\t\t\t(void)0;|'
mutate "the last row is not printed"          's|for (i = 0; i < b.w \* b.h; i++)\n\t{\n\t\tputchar|XX|; s|^\tfor (i = 0; i < b.w \* b.h; i++)$|\tfor (i = 0; i < b.w * b.h - b.w; i++)|'
mutate "a board with four arguments is run"   's|if (ac != 4)|if (ac < 4)|'
mutate "a board with two arguments is run"    's|if (ac != 4)|if (ac == 0)|'

echo
echo "  -- memory --"
mutate "the board is never freed"             's|return (free(b.cells), 0);|return (0);|'
mutate "the old generation is leaked"         's|^\t\tfree(b.cells);$|\t\t(void)0;|'
mutate "the old generation is freed twice"    's|^\t\tfree(b.cells);$|\t\tfree(b.cells); free(b.cells);|'
mutate "the calloc result is not checked"     's|^\tif (!b.cells)\n\t\treturn (1);||; s|^\tif (!b.cells)$|\tif (0)|'

echo
echo "-----------------------------"
if [ $survived -eq 0 ]; then
    printf '\033[32m%d/%d mutants killed\033[0m\n' "$killed" "$((killed + survived))"
    exit 0
fi
printf '\033[1;31m%d/%d mutants killed, %d survived\033[0m\n' \
       "$killed" "$((killed + survived))" "$survived"
exit 1
