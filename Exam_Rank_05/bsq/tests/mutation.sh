#!/bin/sh
# Mutation testing: a test suite is only worth what it catches. Each mutant
# below injects one realistic bsq bug -- a tie-break that picks the wrong
# square, a validation rule that stops being enforced, a fill loop that is off
# by one, a leak. Every mutant must make the suite FAIL. A mutant that survives
# means the suite has a blind spot.
#
# Kill channels, tried in order: the compiler, then the unit cases, then the
# fuzzer, then valgrind. The valgrind pass runs only for mutants the first
# three let through, which is what makes the memory mutants killable without
# paying for valgrind 30 times.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/../solution
WORK=${TMPDIR:-/tmp}/bsq-mutants.$$
CC=${CC:-cc}
FLAGS="-Wall -Wextra"

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

killed=0
survived=0

# A small corpus for the valgrind pass: one valid map, one invalid map, and a
# map with no empty cell at all.
mkdir -p "$WORK/corpus"
printf '4 . o x\n....\n.o..\n....\n..o.\n' > "$WORK/corpus/ok.map"
printf '2 . o x\n..\n.\n'                  > "$WORK/corpus/bad.map"
printf '2 . o x\noo\noo\n'                 > "$WORK/corpus/full.map"
printf 'not a map at all\n....\n'          > "$WORK/corpus/head.map"

# describe / sed script
mutate() {
    name=$1
    edit=$2

    rm -rf "$WORK/m"
    mkdir -p "$WORK/m"
    sed "$edit" "$SRC/bsq.c" > "$WORK/m/bsq.c"

    if cmp -s "$WORK/m/bsq.c" "$SRC/bsq.c"; then
        printf '  \033[1;33m[??]\033[0m %-50s mutation did not apply\n' "$name"
        survived=$((survived + 1))
        return
    fi

    if ! $CC $FLAGS "$WORK/m/bsq.c" -o "$WORK/m/bsq" 2>"$WORK/m/build.log"; then
        printf '  \033[32m[killed]\033[0m %-48s rejected at compile time\n' "$name"
        killed=$((killed + 1))
        return
    fi

    units_out=$(cd "$HERE" && sh -c 'python3 test_units.py "$0" 2>&1' "$WORK/m/bsq"); units_rc=$?
    fuzz_out=$(cd "$HERE" && sh -c 'python3 test_fuzz.py "$0" --maps 1200 --big 8 2>&1' "$WORK/m/bsq"); fuzz_rc=$?

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
    if ! valgrind -q --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
            "$WORK/m/bsq" "$WORK/corpus/ok.map" "$WORK/corpus/bad.map" \
            "$WORK/corpus/full.map" "$WORK/corpus/head.map" >/dev/null 2>"$WORK/m/vg.log"; then
        printf '  \033[32m[killed]\033[0m %-48s by valgrind\n' "$name"
        killed=$((killed + 1))
        return
    fi
    if ! valgrind -q --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
            "$WORK/m/bsq" < "$WORK/corpus/ok.map" >/dev/null 2>>"$WORK/m/vg.log"; then
        printf '  \033[32m[killed]\033[0m %-48s by valgrind (stdin)\n' "$name"
        killed=$((killed + 1))
        return
    fi

    printf '  \033[1;31m[SURVIVED]\033[0m %-46s suite has a blind spot here\n' "$name"
    survived=$((survived + 1))
}

echo "Mutation testing bsq against the suite"
echo

echo "  -- the square itself --"
mutate "tie-break keeps the last square, not the first" 's|if (v > best)|if (v >= best)|'
mutate "the recurrence forgets the diagonal"            's|if (diag < v)\n|XX|; s|^\t\t\t\tif (diag < v)$|\t\t\t\tif (0)|'
mutate "the recurrence takes a max, not a min"          's|int\tv = up < dp\[j\] ? up : dp\[j\];|int\tv = up > dp[j] ? up : dp[j];|'
mutate "the square is one cell too small"               's|dp\[j + 1\] = ++v;|dp[j + 1] = v;|'
mutate "the square is one cell too large"               's|dp\[j + 1\] = ++v;|dp[j + 1] = v + 2;|'
mutate "diag picks up the value just written"           's|diag = up;|diag = dp[j + 1];|'
mutate "an obstacle does not reset the run"             's|dp\[j + 1\] = 0;|dp[j + 1] = up;|'
mutate "the fill starts one row too low"                's|for (int i = br - best + 1; i <= br; i++)|for (int i = br - best + 2; i <= br; i++)|'
mutate "the fill stops one column early"                's|for (int j = bc - best + 1; j <= bc; j++)|for (int j = bc - best + 1; j < bc; j++)|'
mutate "the fill transposes the square"                 's|b->row\[i\]\[j\] = b->full;|b->row[j][i] = b->full;|'
mutate "the fill uses the empty character"              's|b->row\[i\]\[j\] = b->full;|b->row[i][j] = b->empty;|'
mutate "the winner is tracked one row off"              's|^\t\t\t\t\tbr = i;$|\t\t\t\t\t\tbr = i + 1;|'
mutate "the winner is tracked one column off"           's|^\t\t\t\t\tbc = j;$|\t\t\t\t\t\tbc = j - 1;|'

echo
echo "  -- map validation --"
mutate "an unknown map character is accepted"     's|^\t\t\t\treturn (free(dp), 0);$|\t\t\t\tdp[j + 1] = 0;|'
mutate "a zero line count is accepted"            's|b->rows < 1|b->rows < 0|'
mutate "the shape of line 1 is not checked"       's|i + 7 != len \|\| ||; s| \|\| s\[i + 6\] != .\\n.||'
mutate "the separator after the count is dropped" 's|s\[i\] != . .$|0|'
mutate "the separator after empty is dropped"     's|s\[i + 2\] != . . \|\| ||'
mutate "the separator after obstacle is dropped"  's|s\[i + 4\] != . . \|\| ||'
mutate "empty and obstacle may be equal"          's|return (b->empty != b->obst \&\& b->empty != b->full \&\& b->obst != b->full);|return (b->empty != b->full \&\& b->obst != b->full);|'
mutate "empty and full may be equal"              's|b->empty != b->full \&\& ||'
mutate "obstacle and full may be equal"           's| \&\& b->obst != b->full||'
mutate "a zero-width line is accepted"            's|if (len < 2 \|\| buf\[len - 1\] != .\\n.)|if (len < 1 \|\| buf[len - 1] != 0x0a)|'
mutate "a missing final newline is accepted"      's|if (len < 2 \|\| buf\[len - 1\] != .\\n.)|if (len < 2)|'
mutate "lines of different lengths are accepted"  's|^\t\tif (len != b->cols)$|\t\tif (0)|'
mutate "every line resets the width"              's|if (i == 0)\n||; s|^\t\tif (i == 0)$|\t\tif (1)|'
mutate "extra lines after the map are accepted"   's|if (i == b->rows \&\& getline(\&buf, \&cap, f) == -1)|if (i == b->rows)|'
mutate "too few lines are accepted"               's|if (i == b->rows \&\& getline|if (i <= b->rows \&\& getline|'

echo
echo "  -- output and arguments --"
mutate "map error goes to stdout"                 's|fputs("map error\\n", stderr);|fputs("map error\\n", stdout);|g'
mutate "map error has no line break"              's|fputs("map error\\n", stderr);|fputs("map error", stderr);|g'
mutate "an unreadable file is silently skipped"   's|^\t\t\tfputs("map error\\n", stderr);$|\t\t\t(void)0;|'
mutate "rows are printed without a line break"    's|fputs("\\n", stdout);||'
# The separator convention is a knob (see README.md), so this mutant only
# makes sense when the suite expects no separator. With SEP=blank the injected
# behaviour is the wanted one, and injecting it would report a false blind spot.
if [ "${SEP:-none}" = none ]; then
    mutate "an extra blank line separates the maps" \
        's|fputs(b.row\[i\], stdout);|fputs(b.row[i], stdout);if(i==b.rows-1)fputs("\\n",stdout);|'
else
    printf '  \033[1;33m[--]\033[0m %-48s skipped: SEP=blank wants this behaviour\n' \
           "an extra blank line separates the maps"
fi
mutate "the last row is not printed"              's|for (int i = 0; i < b.rows; i++)\n\t\t\t{|XX|; s|^\t\tfor (int i = 0; i < b.rows; i++)$|\t\tfor (int i = 0; i < b.rows - 1; i++)|'
mutate "stdin is not read when there is no file"  's|bsq(stdin);|(void)0;|'
mutate "only the first file argument is handled"  's|for (int i = 1; i < argc; i++)|for (int i = 1; i < 2; i++)|'

echo
echo "  -- memory --"
mutate "the row array is never freed"             's|^\tfree_rows(b.row, b.rows);$|\t(void)0;|'
mutate "the last row is leaked"                   's|free_rows(b.row, b.rows);|free_rows(b.row, b.rows - 1);|'
mutate "the getline buffer is leaked on error"    's|return (free(buf), 0);|return (0);|g'
mutate "the dp row is leaked"                     's|^\tfree(dp);$|\t(void)0;|'
mutate "the file is never closed"                 's|fclose(f);|(void)0;|'
mutate "a row is freed twice"                     's|b->row\[i++\] = buf;|b->row[i++] = buf; if (i > 1) b->row[i - 2] = buf;|'

echo
echo "-----------------------------"
if [ $survived -eq 0 ]; then
    printf '\033[32m%d/%d mutants killed\033[0m\n' "$killed" "$((killed + survived))"
    exit 0
fi
printf '\033[1;31m%d/%d mutants killed, %d survived\033[0m\n' \
       "$killed" "$((killed + survived))" "$survived"
exit 1
