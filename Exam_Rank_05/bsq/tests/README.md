# bsq test harness

Nothing in here is graded. The exam only wants `../solution/bsq.c`; this
directory exists to prove that file is right.

```sh
make test        # everything below, in order  (SEP=blank for the other convention)
make units       # deterministic edge cases      (ARGS=-v to see each check)
make fuzz        # differential fuzzing          (ARGS="--random")
make subject     # run the subject's own example, diff against expected_subject.txt
make compile     # warning matrix, allowed functions, process behaviour
make mutants     # prove the suite actually catches bugs
make sanitize    # ASan + UBSan
make valgrind    # leaks and invalid accesses
make soak        # 40 seeds   (SEEDS=200 make soak for more)
```

`make soak` takes two knobs, both read from the environment as well as the
command line, so the same target serves a quick regression and an overnight
run:

```sh
make soak SEEDS=500
SEEDS=500 make soak                # same thing, e.g. from ../../run_all.sh
make soak SEEDS=200 FUZZARGS="--maps 40000 --big 200"
```

Every seed is reproducible on its own: a failure prints the seed, and
`python3 test_fuzz.py build/bsq --seed N` replays exactly that campaign.

`../solution/bsq.c` is always compiled with the moulinette's own command,
`cc -Wall -Wextra -Werror` — **no `-std`**, so the compiler's default applies
exactly as it will on the exam machine. Only the test code needs anything
more.

bsq is a whole program rather than a class, so every layer here is black-box:
a map goes in, stdout and stderr come out, and both are compared exactly.

## What each layer is for

### `test_units.py` — deterministic edge cases

123 checks. **Every expectation in this file is written out by hand from
`../subject.txt`** — none of it comes from the oracle or from running the
program. That is what makes this layer an independent check on
`test_fuzz.py`, which does use the oracle.

Each fixed map is run **twice**, once as a file argument and once on stdin,
because the subject requires both and they take different code paths.

The parts worth knowing about:

- **The tie-break, in both directions.** "the square closest to the top of the
  map, then the one that's most to the left": a 2×3 board of empties has two
  2×2 answers side by side and a 3×2 board has two stacked, and each must
  pick the right one. A 2×2 board with one corner obstacle has three
  candidate 1×1 squares and must pick `(0,1)`.
- **No square at all.** A map with no empty cell must be printed back
  unchanged, not left blank and not filled anywhere.
- **A map character may be a space, or a digit.** The first line is read by
  position, not split on whitespace, so `2   o x` declares the space as the
  empty character. Splitting on whitespace gets this wrong, and so does
  `sscanf("%d %c %c %c")`.
- **Every validation rule from the subject gets its own case**: zero or
  negative line counts, a missing or duplicated character, tabs instead of
  spaces, doubled spaces, a non-space separator in each of the three
  positions, trailing garbage, lines of unequal length, too few or too many
  lines, an empty line, a zero-width map, the *full* character appearing in
  the map, an unknown character, a stray `\r`, and a missing final newline.
- **Arguments**: one file, two valid files (whose outputs concatenate with no
  blank line between them), valid-then-invalid in both orders, the same file
  twice, a file that does not exist, a directory, and no arguments at all.
- **Larger boards**: 200×200 all empty, a 60×60 board with one central
  obstacle whose answer is provably a 30×30 square at the top left, a
  1×5000 row and a 5000×1 column.

### `oracle.py` — the reference

A second implementation of bsq written from the subject text rather than from
`bsq.c`. It contains **two** solvers:

- `biggest_brute` tries every top-left corner and grows a square out of it,
  checking every cell it covers. Slow, and obviously correct by reading it.
- `biggest_dp` is the O(rows × cols) recurrence.

The fuzzer runs both on every map small enough to afford it and requires them
to agree, so the oracle is itself under test rather than being trusted.

Validation is modelled on `getline` rather than on `str.split`, because the
distinction matters: a map file that ends without a newline, or one whose
first line has no newline at all, has to be rejected, and only a
getline-shaped reader sees that.

### `test_fuzz.py` — differential fuzzing

Random maps go through the program and through the oracle, and the two answers
must match exactly on **both** stdout and stderr. Roughly 55% well-formed maps,
35% deliberately corrupted, 10% pure noise.

Well-formed maps are drawn from eight shapes — all empty, all obstacles, one
obstacle, checkerboard, a diagonal wall, an obstacle border, two equal-sized
empty blocks placed as tie-break bait, and a random density — with the three
map characters sampled from every printable character including the space.

Corrupted maps are a well-formed map with exactly one of twenty specific
injuries: a dropped final newline, a grown or shrunk line, a bad character, a
duplicated header character, a wrong line count, a tab separator, a truncation
at a random offset, and so on. **The oracle decides whether the result is
actually invalid** — several of these injuries leave a legal map, which is
exactly the interesting case.

Maps are fed in **batches of 25 as arguments to one process**. That is much
faster than one process per map, and it makes every run a heavy test of the
multi-map requirement: the concatenation of the batch's answers must equal the
concatenation of the oracle's, with each `map error` landing on stderr in the
right order. When a batch differs, the fuzzer re-runs its maps one at a time
to report the single one that disagrees.

```sh
python3 test_fuzz.py build/bsq --seed 7 --maps 50000 --batch 40
python3 test_fuzz.py build/bsq --random
```

### `mutation.sh` — testing the tests

A passing suite proves nothing until you know it can fail. This injects 42
one-line bugs into copies of `bsq.c` — a tie-break that keeps the last square
instead of the first, a recurrence that drops the diagonal term, a fill loop
off by one in each direction, every validation rule switched off one at a
time, `map error` sent to stdout, and six memory bugs — and requires every one
of them to be caught. All 42 are currently killed.

Kill channels are tried in order: **the compiler, then the unit cases, then
the fuzzer, then valgrind**. The valgrind pass runs only for mutants the first
three let through, which is what makes leaks and double-frees killable without
paying for valgrind 42 times.

One thing the mutant list deliberately does *not* contain is a single-line
change to the first line's shape checks. `hl < 8`, `i + 7 != len` and
`s[i + 6] != '\n'` overlap: `getline` stops at the first newline, so the
newline is always at `len - 1`, and removing any one of the three on its own
changes nothing observable. The list mutates them as a group instead — an
equivalent mutant would report as a blind spot that is not one.

### `compile_checks.sh`

- **The submission, rebuilt in isolation.** `*.c` and `*.h` are copied out of
  `../solution/` into an empty directory and built with
  `cc -Wall -Wextra -Werror *.c`, then run against the subject's example. This
  is the check that matters most: the moulinette compiles *every* `.c` and
  `.h` in the directory you hand in, so a stray file there is a `-Werror`
  error or a duplicate `main`, and a solution that quietly needs a file from
  somewhere else does not build at all.
- **Warning matrix**: gcc and clang, each at `c99/c11/c17/gnu99`, with the exam
  flags plus `-pedantic-errors -Wshadow -Wconversion -Wsign-conversion
  -Wcast-qual -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes
  -Wformat=2`. (`c89` is not in the matrix: `bsq.c` declares loop variables
  inside `for`, which is C99.)
- **The allowed-functions rule, read off the symbol table.** The subject
  allows exactly `malloc calloc realloc free fopen fclose getline fscanf fputs
  fprintf stderr stdout stdin errno`; `nm -u` on the linked binary must show
  nothing else. The program currently links `calloc fclose fopen fputs free
  getline`. The source is grepped as well, in case a macro hides a call from
  the symbol table.
- **The expected-files rule**: the solution directory holds only `*.c` and
  `*.h`, the program builds from `*.c` alone, and there is exactly one `main`.
- **Process behaviour**: no invocation may die of a signal; a solved map goes
  to stdout and nothing else does; `map error` goes to stderr and nothing else
  does; a map arriving through a pipe works (the reader must not need to seek);
  the same file given twice is solved twice.
- **valgrind over a small corpus** covering the valid path, the
  invalid-in-the-body path, the invalid-header path, the no-empty-cell path
  and the missing-file path.

### `maps/subject.map` and `expected_subject.txt`

The example printed in `../subject.txt`, and its answer, both copied out
verbatim. (The subject's own listing wraps row 3 across two lines; it is one
27-character row.) `make subject` also asserts that the answer is a 7×7 block
at rows 0–6, columns 5–11 and that exactly 49 cells changed, so the diff is
backed by a structural claim and not only by a byte comparison.

## The one genuine ambiguity, and what I found out about it

> When your program receives more than one map in argument, each solution or
> "map error" must be followed by a line break.

Every line this program prints already ends in `\n`, so **one** reading is that
consecutive maps follow each other directly with no blank line between them.
The **other** is that an extra `\n` separates them. The sentence supports both.

I went looking for a tiebreaker and did not find one. What I did find:

- **The subject text is authentic.** An independent copy in
  [fbkeskin/42-exam-rank-05](https://github.com/fbkeskin/42-exam-rank-05) is
  byte-identical to `../subject.txt` — same allowed functions, same
  `"map error"` on stderr, same space-separated first line.
- **That repo's solution takes the opposite reading.** Its `main.c` ends each
  iteration with `if (i < argc - 1) fprintf(stdout, "\n");` — a blank line
  between maps, on stdout, emitted even when the map errored.
- **A different bsq subject is also in circulation.** The one in
  [Mohaben-1/42-Exam-Rank-05](https://github.com/Mohaben-1/42-Exam-Rank-05) is
  a *different exercise wearing the same name*: one map only, `"Error: invalid
  map"` on **stdout**, a first line with **no spaces** (`9.ox`), and `printf`
  allowed instead of `fputs`/`stderr`. Nothing here would satisfy it. Check
  which subject you are handed before you start.

So the honest position is: two readings, both live, no authority available.
Rather than guess, the suite validates **either one**.

### Switching conventions

`SEP` is a knob on the whole behavioural suite:

```sh
make test                 # no separator between maps  (the default)
make test SEP=blank       # a blank line between maps
```

`SEP` reaches `test_units.py` (the multi-map argument cases), `test_fuzz.py`
(every batch of 25 maps is compared with the separator in place), `sanitize`
and `soak`. `mutation.sh` reads it too, and skips the *"an extra blank line
separates the maps"* mutant when `SEP=blank`, because there the injected
behaviour is the wanted one.

The switch is verified in both directions — the default binary passes
`--sep none` and fails `--sep blank`, and a variant built the other way does
the reverse, for both units and fuzz.

### The one line in `bsq.c`

To adopt the other convention, the loop in `main` becomes:

```c
	for (int i = 1; i < argc; i++)
	{
		FILE	*f = fopen(argv[i], "r");

		if (!f)
			fputs("map error\n", stderr);
		else
		{
			bsq(f);
			fclose(f);
		}
		if (i < argc - 1)
			fputs("\n", stdout);
	}
```

then run `make test SEP=blank`. The `continue` has to go, so that a map that
could not even be opened still takes part in the separation — which is what
the reference solution above does.

### Why the default is what it is

The subject uses the same construction twice. For the error case it says
*display "map error" ... followed by a line break*, and everyone agrees that
means `"map error\n"` — one newline in total, not two. Reading the multi-map
sentence the same way makes "the solution" the grid, and the mandated line
break the one that terminates its last row. That is the current behaviour, and
it is why the sentence exists at all: so that two maps cannot end up sharing a
line. It is an argument, not a proof.

## Known limits

`parse_header` trusts the announced line count up to 200 000 000 before
rejecting it. A file whose first line says `200000000 . o x` therefore asks
`calloc` for 1.6 GB before discovering there are not that many lines. The
observable behaviour is still `map error` — `calloc` fails, or the row loop
runs out of lines — so this is a resource limit rather than a wrong answer,
and the fuzzer stays well below it on purpose.
