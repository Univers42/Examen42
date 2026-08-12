#!/bin/sh
# Checks that can only be made on the source and on the linked binary:
#   1. the warning matrix (several compilers x several standards)
#   2. the allowed-functions rule from ../subject.txt, read off the symbol table
#   3. the expected-files rule (*.c *.h, and the program builds from them alone)
#   4. behaviour that is a property of the process rather than of one map:
#      exit status, a closed stdout, output going to the right stream

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/../solution
WORK=${TMPDIR:-/tmp}/bsq-compile.$$

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

fails=0
ok()   { printf '  \033[32m[ok]\033[0m %s\n' "$1"; }
bad()  { printf '  \033[1;31m[KO]\033[0m %s\n' "$1"; fails=$((fails + 1)); }

# --------------------------------------------------------------------------
printf '\n\033[36m== Warning matrix ==\033[0m\n'

# -Wall -Wextra -Werror is what the exam uses; the rest is extra pressure.
# bsq.c uses C99 declarations in for-loops, so c89/c90 are not in the matrix.
STRICT="-Wall -Wextra -Werror -pedantic-errors -Wshadow -Wconversion -Wsign-conversion -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wunused -Wformat=2"

for cc in gcc clang; do
    command -v $cc >/dev/null 2>&1 || { printf '  \033[1;33m[--]\033[0m %s not installed\n' "$cc"; continue; }
    for std in c99 c11 c17 gnu99; do
        log=$($cc -std=$std $STRICT -c "$SRC/bsq.c" -o /dev/null 2>&1)
        if [ -z "$log" ]; then
            ok "$cc -std=$std  (exam flags + shadow/conversion/prototypes)"
        else
            bad "$cc -std=$std"
            printf '%s\n' "$log" | sed 's/^/       /' | head -12
        fi
    done
done

# --------------------------------------------------------------------------
printf '\n\033[36m== Submission: the expected files, on their own ==\033[0m\n'

# "Expected files : *.c *.h". The moulinette takes every .c and .h in the
# directory you hand in and compiles the lot with its own command -- so a
# stray file there is compiled too: a second main() is a link error, and any
# warning in it is a -Werror error. This rebuilds the submission in a clean
# directory to prove that nothing else is needed and nothing else is swept in.
SUBMIT=$WORK/submit
rm -rf "$SUBMIT"
mkdir -p "$SUBMIT"
for f in "$SRC"/*.c "$SRC"/*.h; do
    [ -e "$f" ] && cp "$f" "$SUBMIT/"
done
printf '  submitting: %s\n' "$(cd "$SUBMIT" && ls | tr '\n' ' ')"

# No -std: the exam command does not pass one, so the compiler's own default
# applies, exactly as it will on the exam machine.
if (cd "$SUBMIT" && cc -Wall -Wextra -Werror *.c -o bsq) 2>"$WORK/submit.log"; then
    ok "cc -Wall -Wextra -Werror *.c   builds from the submitted files alone"
else
    bad "the submitted files do not build with the moulinette's own command"
    sed 's/^/       /' "$WORK/submit.log" | head -20
fi

if [ -x "$SUBMIT/bsq" ]; then
    if "$SUBMIT/bsq" "$HERE/maps/subject.map" 2>/dev/null \
            | diff -q - "$HERE/expected_subject.txt" >/dev/null; then
        ok "and that program solves the subject's own example"
    else
        bad "the submission build answers the subject's example differently"
    fi
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== Allowed functions ==\033[0m\n'

# ../subject.txt:
#   Allowed functions and globals: malloc, calloc, realloc, free, fopen,
#   fclose, getline, fscanf, fputs, fprintf, stderr, stdout, stdin, errno
ALLOWED="malloc calloc realloc free fopen fclose getline fscanf fputs fprintf stderr stdout stdin errno"

cc -std=c99 -Wall -Wextra -Werror "$SRC/bsq.c" -o "$WORK/bsq" 2>"$WORK/build.log" || {
    bad "bsq.c does not build with the exam's flags"
    sed 's/^/       /' "$WORK/build.log" | head -12
}

if [ -x "$WORK/bsq" ]; then
    # Every symbol the program imports from libc, with the version tag and the
    # glibc-internal underscore names dropped.
    nm -u "$WORK/bsq" 2>/dev/null \
        | sed 's/@.*//; s/^ *[Uw] *//' \
        | grep -v '^__' | grep -v '^_ITM' | grep -v '^$' | sort -u > "$WORK/used"

    forbidden=""
    while read -r sym; do
        found=0
        for a in $ALLOWED; do
            [ "$sym" = "$a" ] && found=1 && break
        done
        [ $found -eq 0 ] && forbidden="$forbidden $sym"
    done < "$WORK/used"

    if [ -z "$forbidden" ]; then
        ok "only allowed functions are linked in: $(tr '\n' ' ' < "$WORK/used")"
    else
        bad "forbidden function(s) linked in:$forbidden"
    fi
fi

# printf/write/read are the three the subject leaves out on purpose, and the
# ones most likely to creep back in. Catch them in the source too, in case a
# macro or an inline hides them from the symbol table.
for f in printf write read putchar puts strlen strcpy memcpy open close exit; do
    if grep -qw "$f" "$SRC/bsq.c" && [ "$f" != "read" ]; then
        # fprintf/fputs contain "printf"/"puts" as substrings; -w keeps that
        # from matching, but read_map legitimately contains "read".
        bad "the source mentions the forbidden function $f"
    fi
done
ok "the source mentions no forbidden function"

# --------------------------------------------------------------------------
printf '\n\033[36m== Expected files ==\033[0m\n'

# "Expected files: *.c *.h". Everything the program needs must be inside the
# solution directory, and it must build from those files alone.
extra=$(find "$SRC" -maxdepth 1 -type f ! -name '*.c' ! -name '*.h' ! -name 'bsq' -printf '%f ' 2>/dev/null)
if [ -z "$extra" ]; then
    ok "the solution directory holds only *.c and *.h"
else
    printf '  \033[1;33m[--]\033[0m other files present (not submitted): %s\n' "$extra"
fi

if cc -std=c99 -Wall -Wextra -Werror "$SRC"/*.c -o "$WORK/all" 2>/dev/null; then
    ok "the program builds from *.c alone"
else
    bad "the program does not build from *.c alone"
fi

# One main, and it is in a .c file.
n=$(grep -cE '^int[[:space:]]*main' "$SRC"/*.c | awk -F: '{s += $NF} END {print s+0}')
if [ "$n" -eq 1 ]; then
    ok "exactly one main()"
else
    bad "$n definitions of main()"
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== Process behaviour ==\033[0m\n'

BSQ=$WORK/bsq
printf '2 . o x\n..\n..\n' > "$WORK/good.map"
printf '2 . o x\n..\n.\n'  > "$WORK/bad.map"

rc_of() { "$@" >/dev/null 2>&1; echo $?; }

# The subject says nothing about the exit status, so the only requirement is
# that the program terminates normally -- in particular it must not die of a
# signal (128+n) on any of these.
for case in "$BSQ $WORK/good.map" "$BSQ $WORK/bad.map" "$BSQ /nonexistent_zz" "$BSQ"; do
    rc=$(rc_of $case < /dev/null)
    if [ "$rc" -lt 128 ]; then
        ok "exits normally (status $rc):  $(basename "$case")"
    else
        bad "died of a signal (status $rc):  $case"
    fi
done

# The solved map goes to stdout and nothing else does; map error goes to
# stderr and nothing else does.
out=$("$BSQ" "$WORK/good.map" 2>/dev/null)
err=$("$BSQ" "$WORK/good.map" 2>&1 >/dev/null)
if [ "$out" = "xx
xx" ] && [ -z "$err" ]; then
    ok "a solved map goes to stdout only"
else
    bad "a solved map wrote to the wrong stream"
fi

out=$("$BSQ" "$WORK/bad.map" 2>/dev/null)
err=$("$BSQ" "$WORK/bad.map" 2>&1 >/dev/null)
if [ -z "$out" ] && [ "$err" = "map error" ]; then
    ok "map error goes to stderr only"
else
    bad "map error wrote to the wrong stream (stdout=$out stderr=$err)"
fi

# A map read from a pipe rather than a seekable file: the reader must not
# depend on being able to rewind.
if [ "$(printf '2 . o x\n..\n..\n' | "$BSQ")" = "xx
xx" ]; then
    ok "reads a map from a pipe"
else
    bad "fails on a piped map"
fi

# The same file given twice must be read twice, independently.
if [ "$("$BSQ" "$WORK/good.map" "$WORK/good.map" | wc -l)" -eq 4 ]; then
    ok "the same file given twice is solved twice"
else
    bad "the same file given twice is not solved twice"
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== valgrind over a small corpus ==\033[0m\n'

if command -v valgrind >/dev/null 2>&1; then
    printf 'not a map\n'       > "$WORK/head.map"
    printf '2 . o x\noo\noo\n' > "$WORK/full.map"
    if valgrind -q --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
           "$BSQ" "$WORK/good.map" "$WORK/bad.map" "$WORK/head.map" "$WORK/full.map" \
           "/nonexistent_zz" >/dev/null 2>"$WORK/vg.log"; then
        ok "no leaks and no invalid accesses across valid, invalid and missing maps"
    else
        bad "valgrind found a problem"
        sed 's/^/       /' "$WORK/vg.log" | head -20
    fi
    if valgrind -q --error-exitcode=1 --leak-check=full --errors-for-leak-kinds=all \
           "$BSQ" < "$WORK/good.map" >/dev/null 2>"$WORK/vg2.log"; then
        ok "no leaks on the stdin path either"
    else
        bad "valgrind found a problem on the stdin path"
        sed 's/^/       /' "$WORK/vg2.log" | head -20
    fi
else
    printf '  \033[1;33m[--]\033[0m valgrind not installed\n'
fi

echo
echo "-----------------------------"
if [ $fails -eq 0 ]; then
    printf '\033[32mall compile-time checks passed\033[0m\n'
    exit 0
fi
printf '\033[1;31m%d compile-time check(s) failed\033[0m\n' "$fails"
exit 1
