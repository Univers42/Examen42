# bigint test harness

Nothing in here is graded. The exam only wants `../bigint.hpp` and
`../bigint.cpp`; this directory exists to prove those two files are right.

```sh
make test        # everything below, in order
make units       # deterministic edge cases      (ARGS=-v to see each check)
make fuzz        # differential + expression fuzzing
make subject     # run the subject's main, diff against expected_subject.txt
make compile     # warning matrix, const-correctness, ODR
make mutants     # prove the suite actually catches bugs
make sanitize    # ASan + UBSan
make valgrind    # leaks and invalid accesses
make soak        # 100 seeds  (SEEDS=500 make soak for more)
```

`make soak` takes two knobs, both read from the environment as well as the
command line, so the same target serves a quick regression and an overnight
run:

```sh
make soak SEEDS=500
SEEDS=500 make soak                # same thing, e.g. from ../../run_all.sh
make soak SEEDS=200 FUZZARGS="--steps 2000000 --exprs 200000"
```

Every seed is reproducible on its own: a failure prints the seed, and
`./build/fuzz --seed N` replays exactly that campaign.

`../bigint.cpp` is always compiled with the exam's own flags
(`-Wall -Wextra -Werror -std=c++98`). Only the test code is allowed to need
anything more.

## What each layer is for

### `test_units.cpp` — deterministic edge cases

3846 checks, grouped by operator. The parts worth knowing about:

- **Normalisation is the invariant.** A bigint must print with no leading
  zeros and must print `0` — not the empty string — for zero. Every operation
  that can break that has its own case: `bigint("0042")`, `0 << 5`,
  `1337 >> 4`, `1050 >> 2`, `9 >>= 1`.
- **Reference identity, not just value.** `+=`, `<<=`, `>>=`, `=` and `++b`
  must return `*this`, so the tests assert `&(a += b) == &a`. An
  implementation returning a copy still passes a naive value check but breaks
  `a += b += c` and `(a += b) += c`.
- **Postfix returns a copy.** `(q++)++` must leave `q` incremented exactly
  once — that fails immediately if postfix returns `*this`.
- **Operands survive.** `a + b`, `a << k` and `a >> k` must not touch either
  side. `operator+` takes its left operand *by value* precisely so that
  `a + b` cannot quietly become `a += b`.
- **Aliasing.** `s += s`, `s = s`, `s = alias_of_s`, `s <<= s`, `s >>= s`,
  `(a += b) += c` and `a += b += c`.
- **Carries, at every width.** `9 + 1`, `99 + 1`, 50 nines `+ 1`, 500 nines
  `+ 1`, 100 nines doubled, and the two cases that exercise the "graft"
  branch of `operator+=`: a carry that has to ripple into digits copied
  straight from the wider operand (`999 + 1001`) and one that must not
  (`999 + 1000000`).
- **Comparison is numeric, not lexicographic.** `9 < 10`,
  `99999999 < 100000000`, plus an exhaustive 24×24 sweep where all six
  relational operators must agree with the same operators on `int`.
- **Two independent references for large values**: `10^n` written as `"1"`
  followed by n zeros, and `10^n - 1` written as n nines. Every large
  expectation is built from those, so it can be checked by reading it rather
  than by trusting the code that produced it. `fib(100)` is included as a
  known 21-digit constant.

### `test_fuzz.cpp` — differential fuzzing

Two campaigns, both comparing `bigint` against a reference model that holds
the same number as **little-endian digits in a `std::vector<int>`**. That is
deliberately the mirror image of bigint's own representation (big-endian, in a
`std::string`, mutated in place through raw `char*`): a model written the same
way round would share bigint's bugs and agree with it for the wrong reason.

1. **Stateful.** Six live `bigint` objects, each shadowed by a model. Each
   step picks one of 17 operations and random operands — deliberately allowing
   the operands to alias, so `v[a] += v[b]` becomes `v[a] += v[a]` a sixth of
   the time. After every step the *entire* state is read back through
   `operator<<` and compared to the model; one step in seventeen additionally
   checks all six relational operators over all 36 ordered pairs.
2. **Expression trees.** Random nested expressions like
   `((a + b) << 3) + (c >> 1)`, evaluated once with `bigint` and once with the
   model. This reaches operator combinations a flat list of operations never
   will.

Random digit strings are weighted towards the shapes that break carry
handling: all nines, all zeros but one, and a run of nines at the low end.

Sizes are capped (400 digits, 60 digits per left shift). `x <<= k` allocates
`k` digits, so an unbounded shift amount would ask for gigabytes rather than
find a bug. Right shifts cost nothing and *are* fuzzed with amounts far larger
than the values they are applied to. A typical run applies ~182k operations and
skips ~6k for the width cap.

```sh
./build/fuzz --seed 7 --steps 500000 --exprs 50000
./build/fuzz --random            # seed from the clock
```

### `mutation.sh` — testing the tests

A passing suite proves nothing until you know it can fail. This injects 44
one-line bugs into copies of `bigint.cpp` / `bigint.hpp` — a dropped carry-out,
a carry ripple that stops one digit early, `<<=` denormalising zero into
`"0000"`, `>>=` erasing from the wrong end, `<` falling back to a plain string
comparison, `!=` aliased to `==`, postfix `++` returning the new value — and
requires every one of them to be caught. All 44 are currently killed, 39 by the
tests and 5 at compile time.

The mutants are copied *together with the test sources* into a scratch
directory, because the tests include `"../bigint.hpp"` relative to their own
location: compiling them in place would silently pick up the pristine header
and miss every mutation living in it — which is most of them, since the class
is header-heavy.

### `compile_checks.sh`

- **The submission, rebuilt in isolation.** The subject collects exactly
  `bigint.hpp` and `bigint.cpp`. Those two are copied into an empty directory
  next to the main the subject ships, built with
  `c++ -Wall -Wextra -Werror -std=c++98`, and run — which proves they are a
  complete program and do not quietly need anything else from `../`.
- **Warning matrix**: g++ and clang++, each at `c++98/03/11/17`, with the exam
  flags plus `-pedantic-errors -Wshadow -Wold-style-cast -Wconversion
  -Wsign-conversion -Wcast-qual -Weffc++`.
- **const-correctness as negative tests**: the subject declares
  `const bigint a(42)`, so `a += 1`, `++a`, `a++`, `a <<= 1`, `a >>= 1` and
  `a = ...` must all be *rejected*, while every operation the subject performs
  on `a` must still be accepted.
- **The string constructor is `explicit`**: `bigint x = std::string("42")`
  must not compile. The `int` constructor is deliberately *not* explicit,
  because the subject writes `(b << 10) + 42` and `d <<= 4`.
- **ODR**: the constructors, both increments and every friend operator are
  defined inside the class body, which makes them implicitly `inline`.
  Including the header from three translation units and linking them proves it.
- The graded files are checked to stand alone: `bigint.cpp` defines no `main`
  (the grader supplies its own), the header compiles with nothing included
  before it, and it survives being included twice.

### `expected_subject.txt`

The output of `../main.cpp`, which is the main the subject ships. Each line was
worked out by hand from the subject's own text (`42 << 3 == 42000`,
`1337 >> 2 == 13`) rather than captured from the implementation, so the diff is
a real check and not a snapshot of whatever the code happened to print.

The one line that surprises people is `b++ = 22`: postfix yields the value from
*before* the increment, so it prints 22 while leaving `b` at 23.

## Known limits of the implementation

Documented here rather than tested, because both are properties of the design
rather than defects:

- `bigint` is **unsigned**. `bigint(int)` clamps a negative seed to `0`; there
  is no subtraction, so a value can never become negative later.
- The shift amount is decoded by `count()`, which saturates above 9 digits.
  A shift left by a 10-digit amount would need terabytes of digits; the
  implementation asks `std::string::append` for `SIZE_MAX` characters, which
  throws `std::length_error` instead of corrupting anything. The fuzzer stays
  well below that on purpose — see the size caps above.
