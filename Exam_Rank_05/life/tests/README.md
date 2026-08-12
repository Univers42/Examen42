# life test harness

Nothing in here is graded. The exam only wants `../life.c` and `../life.h`;
this directory exists to prove those two files are right.

```sh
make test        # everything below, in order
make units       # deterministic edge cases      (ARGS=-v to see each check)
make fuzz        # differential fuzzing          (ARGS="--random")
make subject     # replay the subject's five examples, diff the transcript
make compile     # warning matrix, allowed functions, process behaviour
make mutants     # prove the suite actually catches bugs
make sanitize    # ASan + UBSan
make valgrind    # leaks and invalid accesses
make soak        # 30 seeds   (SEEDS=150 make soak for more)
```

`make soak` takes two knobs, both read from the environment as well as the
command line, so the same target serves a quick regression and an overnight
run:

```sh
make soak SEEDS=500
SEEDS=500 make soak                # same thing, e.g. from ../../run_all.sh
make soak SEEDS=200 FUZZARGS="--cases 30000 --long 200"
```

Every seed is reproducible on its own: a failure prints the seed, and
`python3 test_fuzz.py build/life --seed N` replays exactly that campaign.

`../life.c` is always compiled with the moulinette's own command,
`cc -Wall -Wextra -Werror` — **no `-std`**, so the compiler's default applies
exactly as it will on the exam machine. Only the test code needs anything
more.

life is a whole program rather than a class, so every layer here is black-box:
dimensions and a command stream go in, a board comes out, and it is compared
exactly — trailing spaces included.

## What each layer is for

### `test_units.py` — deterministic edge cases

82 checks. **Every expectation in this file is written out by hand from
`../subject.txt`** — none of it comes from the oracle or from running the
program. That is what makes this layer an independent check on
`test_fuzz.py`, which does use the oracle. Boards are written with `.` for a
dead cell so the trailing spaces stay visible in the source; `grid()` turns
them back into the spaces the program has to print.

The parts worth knowing about:

- **All five examples from the subject**, plus the blinker at eight more
  iteration counts: it has period 2, so every even count must come back to the
  vertical bar and every odd one to the horizontal.
- **When `x` draws.** Lowering the pen draws the cell it is *already standing
  on* — `x` alone on an empty board lights `(0,0)`. That single fact is what
  makes the subject's first example come out as a ring, and it is the one
  thing most implementations get wrong.
- **The pen outside the board.** "each cell outside of the array will be
  considered dead", but the pen keeps its position out there and resumes
  drawing when it comes back. `wwwaaaxsssddd` lowers the pen three cells above
  and three cells left of a 3×3 board and draws exactly one cell, on the last
  command.
- **Characters that are not commands** — a trailing newline, capitals,
  digits, punctuation, a tab, a NUL byte — must move nothing and toggle
  nothing.
- **The rules, one at a time**: a lone cell dies, a pair dies, an L of three
  becomes a block, a block is stable for 7 generations, and a solid 3×3 block
  thins to four corners plus two births (its centre has eight neighbours, its
  edge midpoints five).
- **The edge really is dead.** A blinker pressed against the left edge does
  *not* oscillate — it collapses to two cells and then to none. A board one
  cell wide still has vertical neighbours, so a run of four keeps its middle
  two.
- **A glider**, checked by its defining property: after 4 generations the same
  shape reappears one cell down and one cell right, and after 8, two. That is
  a property of the pattern rather than of this implementation, so it is a
  genuine outside check.
- **The output format**: a dead board still prints its full width in spaces,
  one newline per row including the last, and a 40×25 board is exactly
  25 × 41 bytes.
- **Arguments**: 0, 1, 2, 4 and 5 of them are all rejected with no output;
  `atoi` semantics (trailing junk, a leading space, a plus sign) are pinned
  down; a negative or non-numeric iteration count means zero generations.

### `oracle.py` — the reference

A second implementation of life written from the subject text rather than from
`life.c`. It contains **two** generation steppers:

- `step_count` is the straightforward one: for each cell, count the eight
  neighbours that exist and are alive, then apply the rule.
- `step_sets` works the other way round — it walks the live cells and adds one
  to each of their neighbours' tallies, so a cell's count is built up rather
  than read off. Nothing about the loop bounds is shared with the first
  version.

The fuzzer runs both on every small board and requires them to agree, so the
oracle is itself under test rather than being trusted.

### `test_fuzz.py` — differential fuzzing

Random boards, iteration counts and command streams go through the program and
through the oracle; stdout must match byte for byte and stderr must be empty.
Four campaigns:

1. **Small boards** (up to 12×12, up to 30 generations). Small boards are
   where the edges dominate, and the edge rule is the one most often got
   wrong. A quarter of the streams are *structured* rather than random: a full
   sweep of every cell, the border, a diagonal, a pen that spends most of its
   life outside the board, and a pen that toggles every other step.
2. **Degenerate boards**: 1-wide, 1-tall and 1×1.
3. **Larger boards** (up to 60×40, command streams up to 1500 characters).
4. **Unusable arguments**: zero and negative dimensions must print nothing and
   must not crash — and, separately, a *negative iteration count* must behave
   exactly like zero, which is a different thing.

Command streams mix the five real commands with characters that must be
ignored — capitals, digits, whitespace, punctuation — at a rate that itself
varies per case.

```sh
python3 test_fuzz.py build/life --seed 7 --cases 50000 --long 200
python3 test_fuzz.py build/life --random
```

### `mutation.sh` — testing the tests

A passing suite proves nothing until you know it can fail. This injects 45
one-line bugs into copies of `life.c` — a pen that starts down, `w`/`s`
swapped, a neighbour count that includes the cell itself, an edge that wraps,
survival on three instead of two, one iteration too many, the line break
placed with the height instead of the width, and five memory bugs — and
requires every one of them to be caught. All 45 are currently killed.

Kill channels are tried in order: **the compiler, then the unit cases, then
the fuzzer, then valgrind, then an allocation failure**. The last one runs the
mutant under `ulimit -v 65536` against a board that needs 400 MB; it is the
only way to reach the `calloc` guards, since `calloc` never fails on a board
small enough to test normally.

### `compile_checks.sh`

- **The submission, rebuilt in isolation.** `*.c` and `*.h` are copied out of
  `../` into an empty directory and built with `cc -Wall -Wextra -Werror *.c`,
  then run against all five subject examples. This is the check that matters
  most: the moulinette compiles *every* `.c` and `.h` in the directory you
  hand in. It is also the check that caught `life_training.c` — a second
  `main` with three `-Werror` warnings of its own, sitting next to `life.c`
  and matching `*.c`. It now lives in `../training/`.
- **Warning matrix**: gcc and clang, each at `c99/c11/c17/gnu99`, with the exam
  flags plus `-pedantic-errors -Wshadow -Wconversion -Wsign-conversion
  -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes
  -Wformat=2`.
- **The allowed-functions rule, read off the symbol table.** The subject
  allows exactly `atoi read putchar malloc calloc realloc free`; `nm -u` on
  the linked binary must show nothing else. The program currently links
  `atoi calloc free putchar read`. The source is grepped as well, in case a
  macro hides a call from the symbol table.
- **The expected-files rule**: the program builds from `life.c` + `life.h`
  alone, there is exactly one `main`, and the header is self-contained and
  safe to include twice.
- **Process behaviour**: no invocation dies of a signal; stderr stays empty; a
  7×4 board is exactly 32 bytes; a command stream with no trailing newline
  works; an empty board prints nothing.
- **Allocation failure**: under a 64 MB address space, a board needing 400 MB
  must exit cleanly rather than dereference a null `calloc`. And a board whose
  `w * h` would overflow an `int` must be refused rather than indexed with a
  wrapped-around counter.
- **valgrind** over six boards, including a 1×1 and an empty command stream.

### `expected_subject.txt` and `run_subject.sh`

`run_subject.sh` replays the five invocations printed in `../subject.txt` and
writes a transcript; `make subject` diffs it against `expected_subject.txt`.

The subject's own listing was written with the trailing blanks stripped, so it
cannot be used verbatim: `O OOO ` in the subject is really `` ` O   OOO  ` ``
on a 10-wide board. `expected_subject.txt` restores them by hand from the
stated dimensions, and asserts that every row is exactly as wide as the board.

## Fixes made to the graded files while building this

- **`life_training.c` was moved to `../training/`.** The subject's expected
  files are `*.c *.h`, so it was being handed in alongside `life.c`: a second
  `main`, and three `-Werror` errors of its own. `cc -Wall -Wextra -Werror *.c`
  failed outright before this move.
- `nb()` is now `static`. It is only used inside `life.c`, and
  `-Wmissing-prototypes` rejects a non-static function with no prototype.
- Three narrowing conversions are now explicit, so the file survives
  `-Wconversion -Wsign-conversion`.
- **`w * h` is checked before it is used.** It is computed as an `int` in four
  places, so `./life 100000 100000 0` used to overflow one — undefined
  behaviour. Dimensions below 1, and boards whose cell count would not fit in
  an `int`, are now refused before anything is allocated.
- **Both `calloc` results are now checked.** The second one is inside the
  generation loop, where a failure would have written through a null pointer
  on a board the program had already accepted.
