#!/bin/sh
# Checks that can only be made on the source and on the linked binary:
#   1. the warning matrix (several compilers x several standards)
#   2. the allowed-functions rule from ../subject.txt, read off the symbol table
#   3. the expected-files rule (*.c *.h, and the program builds from them alone)
#   4. behaviour that belongs to the process rather than to one board: exit
#      status, streams, a board so large that calloc has to fail

set -u

HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$HERE/..
WORK=${TMPDIR:-/tmp}/life-compile.$$

mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT

fails=0
ok()   { printf '  \033[32m[ok]\033[0m %s\n' "$1"; }
bad()  { printf '  \033[1;31m[KO]\033[0m %s\n' "$1"; fails=$((fails + 1)); }

# --------------------------------------------------------------------------
printf '\n\033[36m== Warning matrix ==\033[0m\n'

# -Wall -Wextra -Werror is what the exam uses; the rest is extra pressure.
STRICT="-Wall -Wextra -Werror -pedantic-errors -Wshadow -Wconversion -Wsign-conversion -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wunused -Wformat=2"

for cc in gcc clang; do
    command -v $cc >/dev/null 2>&1 || { printf '  \033[1;33m[--]\033[0m %s not installed\n' "$cc"; continue; }
    for std in c99 c11 c17 gnu99; do
        log=$($cc -std=$std $STRICT -I"$SRC" -c "$SRC/life.c" -o /dev/null 2>&1)
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
if (cd "$SUBMIT" && cc -Wall -Wextra -Werror *.c -o life) 2>"$WORK/submit.log"; then
    ok "cc -Wall -Wextra -Werror *.c   builds from the submitted files alone"
else
    bad "the submitted files do not build with the moulinette's own command"
    sed 's/^/       /' "$WORK/submit.log" | head -20
fi

if [ -x "$SUBMIT/life" ]; then
    if "$HERE/run_subject.sh" "$SUBMIT/life" 2>/dev/null \
            | diff -q - "$HERE/expected_subject.txt" >/dev/null; then
        ok "and that program reproduces all five subject examples"
    else
        bad "the submission build differs on the subject's examples"
    fi
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== Allowed functions ==\033[0m\n'

# ../subject.txt:
#   Allowed functions: atoi, read, putchar, malloc, calloc, realloc, free
ALLOWED="atoi read putchar malloc calloc realloc free"

cc -std=c99 -Wall -Wextra -Werror -I"$SRC" "$SRC/life.c" -o "$WORK/life" 2>"$WORK/build.log" || {
    bad "life.c does not build with the exam's flags"
    sed 's/^/       /' "$WORK/build.log" | head -12
}

if [ -x "$WORK/life" ]; then
    nm -u "$WORK/life" 2>/dev/null \
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

# printf and write are the two most likely to creep in behind putchar.
for f in printf fprintf puts fputs write open close exit memset strlen; do
    if grep -qw "$f" "$SRC/life.c"; then
        bad "the source mentions the forbidden function $f"
    fi
done
ok "the source mentions no forbidden function"

# --------------------------------------------------------------------------
printf '\n\033[36m== Expected files ==\033[0m\n'

# "Expected files : *.c *.h" -- and the program must build from those alone.
if cc -std=c99 -Wall -Wextra -Werror -I"$SRC" "$SRC/life.c" -o "$WORK/only" 2>/dev/null; then
    ok "the program builds from life.c + life.h alone"
else
    bad "the program does not build from life.c + life.h alone"
fi

n=$(grep -cE '^int[[:space:]]*main' "$SRC/life.c")
if [ "$n" -eq 1 ]; then
    ok "exactly one main()"
else
    bad "$n definitions of main()"
fi

printf '#include "life.h"\n#include "life.h"\nint main(void){ t_board b; b.w = 0; (void)b; return 0; }\n' > "$WORK/twice.c"
if cc -std=c99 -Wall -Wextra -Werror -I"$SRC" "$WORK/twice.c" -o "$WORK/twice" 2>/dev/null; then
    ok "life.h is self-contained and safe to include twice"
else
    bad "life.h is not self-contained, or has no include guard"
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== Process behaviour ==\033[0m\n'

LIFE=$WORK/life

# The subject gives no exit status, so the only requirement is that nothing
# dies of a signal (128+n).
for args in "5 5 0" "1 1 0" "3 3 10" "0 0 0" "-1 -1 -1" "" "5" "5 5" "5 5 0 0"; do
    printf 'xdsdsa\n' | "$LIFE" $args >/dev/null 2>&1
    rc=$?
    if [ $rc -lt 128 ]; then
        ok "exits normally (status $rc):  ./life $args"
    else
        bad "died of a signal (status $rc):  ./life $args"
    fi
done

# Everything goes to stdout; stderr stays empty.
err=$(printf 'xddss\n' | "$LIFE" 5 5 2 2>&1 >/dev/null)
if [ -z "$err" ]; then
    ok "nothing is written to stderr"
else
    bad "something was written to stderr: $err"
fi

# The board is exactly h lines of w characters.
out=$(printf 'x\n' | "$LIFE" 7 4 0 | wc -c)
if [ "$out" -eq 32 ]; then
    ok "a 7x4 board prints 4 * (7 + 1) = 32 bytes"
else
    bad "a 7x4 board printed $out bytes, expected 32"
fi

# Reading commands from a pipe with no trailing newline.
if [ "$(printf 'xd' | "$LIFE" 3 1 0)" = "OO " ]; then
    ok "a command stream with no trailing newline"
else
    bad "a command stream with no trailing newline is mishandled"
fi

# Nothing is read when the arguments are unusable, and nothing is printed.
if [ -z "$(printf 'xxxx' | "$LIFE" 0 0 0)" ]; then
    ok "an empty board prints nothing"
else
    bad "an empty board printed something"
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== Allocation failure ==\033[0m\n'

# The only way to reach the calloc guards is to make calloc fail. 64 MB of
# address space and a board that needs 400 MB does it.
(ulimit -v 65536 2>/dev/null; printf 'x\n' | "$LIFE" 20000 20000 1) >/dev/null 2>&1
rc=$?
if [ $rc -gt 128 ]; then
    bad "crashed (status $rc) when calloc failed -- the result is not checked"
else
    ok "exits cleanly (status $rc) when calloc cannot provide the board"
fi

# A board whose cell count would overflow an int must be refused rather than
# indexed with a wrapped-around counter.
"$LIFE" 100000 100000 0 </dev/null >"$WORK/huge.out" 2>&1
rc=$?
if [ $rc -gt 128 ]; then
    bad "crashed (status $rc) on a board whose w * h overflows an int"
elif [ -s "$WORK/huge.out" ]; then
    bad "printed something for a board whose w * h overflows an int"
else
    ok "a board whose w * h overflows an int is refused, quietly"
fi

# --------------------------------------------------------------------------
printf '\n\033[36m== valgrind over a small corpus ==\033[0m\n'

if command -v valgrind >/dev/null 2>&1; then
    vg=0
    for run in "5 5 0|sdxddssaaww" "10 6 4|sdxssdswdxddddsxaadwxwdxwaa" "3 3 7|dxss" \
               "1 1 1|x" "20 20 3|xddddssssaaaawwww" "4 4 0|"; do
        args=${run%%|*}
        cmds=${run##*|}
        printf '%s\n' "$cmds" | valgrind -q --error-exitcode=1 --leak-check=full \
            --errors-for-leak-kinds=all "$LIFE" $args >/dev/null 2>"$WORK/vg.log" || {
            bad "valgrind found a problem on ./life $args"
            sed 's/^/       /' "$WORK/vg.log" | head -14
            vg=1
        }
    done
    [ $vg -eq 0 ] && ok "no leaks and no invalid accesses across six boards"
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
