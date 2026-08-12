#!/bin/sh
# One verdict for the whole rank.
#
# Every exercise's harness exposes the same targets, so this just runs them all
# and tallies the result.
#
#   ./run_all.sh             compile checks, subject output, units, fuzz, mutants
#   ./run_all.sh full        ... and also sanitize, valgrind and soak
#   ./run_all.sh marathon    five long fuzz campaigns, in parallel
#   ./run_all.sh <layer>     one layer everywhere, e.g. ./run_all.sh valgrind
#
# SEEDS and FUZZARGS are read from the environment by every harness, so a
# heavier run of one layer everywhere is just:
#
#   SEEDS=500 ./run_all.sh soak
#
# A failing layer does not stop the run: everything is attempted so the summary
# at the end shows the whole picture, and the exit status is non-zero if
# anything failed.

set -u

HERE=$(cd "$(dirname "$0")" && pwd)

SUITES="vect2/solution/tests bigint/tests polyset/tests bsq/tests life/tests"

GREEN='\033[32m'
RED='\033[1;31m'
CYAN='\033[36m'
YELLOW='\033[1;33m'
OFF='\033[0m'

LOGDIR=$HERE/.run_all_logs

# --------------------------------------------------------------------------
# marathon: the five soak campaigns at a much larger volume, run in parallel.
# The sizes differ per exercise because a "seed" costs very different amounts
# -- a life seed drives 20 000 board simulations through 20 000 subprocesses,
# a vect2 seed is 300 000 in-process operations.

marathon_one() {
    name=$1
    dir=$2
    shift 2
    (cd "$HERE/$dir" && make --no-print-directory soak "$@") \
        > "$LOGDIR/$name.marathon.log" 2>&1
    echo "$? $name" >> "$LOGDIR/rc"
}

run_marathon() {
    printf "${CYAN}==================== marathon ====================${OFF}\n"
    printf "  five long fuzz campaigns, in parallel -- around 12 minutes on 6 cores\n\n"
    : > "$LOGDIR/rc"

    marathon_one vect2   vect2/solution/tests \
        SEEDS=600 FUZZARGS="--steps 300000 --trees 30000" &
    marathon_one bigint  bigint/tests \
        SEEDS=300 FUZZARGS="--steps 500000 --exprs 50000" &
    marathon_one polyset polyset/tests \
        SEEDS=100 FUZZARGS="--bags 30000 --sets 30000 --views 20000" &
    marathon_one bsq     bsq/tests \
        SEEDS=100 FUZZARGS="--maps 20000 --big 100" &
    marathon_one life    life/tests \
        SEEDS=40  FUZZARGS="--cases 20000 --long 100" &
    wait

    bad=0
    while read -r rc name; do
        if [ "$rc" = 0 ]; then
            last=$(tail -1 "$LOGDIR/$name.marathon.log" \
                   | sed 's/\033\[[0-9;]*m//g; s/^ *\[ok\] *//')
            printf "  ${GREEN}[ok]${OFF}     %-8s %s\n" "$name" "$last"
        else
            printf "  ${RED}[FAILED]${OFF} %-8s -- see %s\n" \
                   "$name" "$LOGDIR/$name.marathon.log"
            tail -20 "$LOGDIR/$name.marathon.log" | sed 's/^/         /'
            bad=$((bad + 1))
        fi
    done < "$LOGDIR/rc"

    echo
    if [ $bad -eq 0 ]; then
        printf "${GREEN}marathon clean${OFF}\n"
        rm -rf "$LOGDIR"
        return 0
    fi
    printf "${RED}%d marathon campaign(s) failed -- logs in %s${OFF}\n" "$bad" "$LOGDIR"
    return 1
}

# --------------------------------------------------------------------------

MODE=${1:-quick}

rm -rf "$LOGDIR"
mkdir -p "$LOGDIR"

case $MODE in
    quick)    LAYERS="test" ;;
    full)     LAYERS="test sanitize valgrind soak" ;;
    marathon) run_marathon; exit $? ;;
    *)        LAYERS=$MODE ;;
esac

failed=0
summary=""

for suite in $SUITES; do
    name=$(printf '%s' "$suite" | cut -d/ -f1)
    printf "\n${CYAN}==================== %s ====================${OFF}\n" "$name"

    for layer in $LAYERS; do
        log=$LOGDIR/$name.$layer.log
        printf '  %-9s ' "$layer"
        start=$(date +%s)
        if (cd "$HERE/$suite" && make --no-print-directory "$layer") > "$log" 2>&1; then
            end=$(date +%s)
            printf "${GREEN}ok${OFF}   (%ss)\n" "$((end - start))"
            summary="$summary$name:$layer=ok "
        else
            end=$(date +%s)
            printf "${RED}FAILED${OFF} (%ss)  -- see %s\n" "$((end - start))" "$log"
            # Show the lines that actually explain it.
            grep -E '\[KO\]|\[SURVIVED\]|FAIL|Error|error:' "$log" | head -12 | sed 's/^/         /'
            summary="$summary$name:$layer=FAILED "
            failed=$((failed + 1))
        fi
    done
done

printf "\n${CYAN}==================== summary ====================${OFF}\n"
for suite in $SUITES; do
    name=$(printf '%s' "$suite" | cut -d/ -f1)
    line=""
    for layer in $LAYERS; do
        case " $summary " in
            *" $name:$layer=ok "*)     line="$line ${GREEN}$layer${OFF}" ;;
            *" $name:$layer=FAILED "*) line="$line ${RED}$layer${OFF}" ;;
            *)                         line="$line ${YELLOW}$layer${OFF}" ;;
        esac
    done
    printf "  %-9s%b\n" "$name" "$line"
done

echo
if [ $failed -eq 0 ]; then
    printf "${GREEN}all %d suite/layer combinations passed${OFF}\n" \
           "$(printf '%s' "$summary" | tr ' ' '\n' | grep -c '=ok')"
    rm -rf "$LOGDIR"
    exit 0
fi
printf "${RED}%d suite/layer combination(s) failed -- logs in %s${OFF}\n" "$failed" "$LOGDIR"
exit 1
