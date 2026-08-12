# vect2 test harness

Nothing in here is graded. The exam only wants `../vect2.cpp` and
`../vect2.hpp`; this directory exists to prove those two files are right.

```sh
make test        # everything below, in order
make units       # deterministic edge cases      (ARGS=-v to see each check)
make fuzz        # differential + expression fuzzing
make subject     # run the subject's main, diff against expected_subject.txt
make compile     # warning matrix, const-correctness, ODR
make mutants     # prove the suite actually catches bugs
make sanitize    # ASan + UBSan
make valgrind    # leaks and invalid accesses
make soak        # 120 seeds  (SEEDS=500 make soak for more)
```

`make soak` takes two knobs, both read from the environment as well as the
command line, so the same target serves a quick regression and an overnight
run:

```sh
make soak SEEDS=500
SEEDS=500 make soak                # same thing, e.g. from ../../run_all.sh
make soak SEEDS=200 FUZZARGS="--steps 2000000 --trees 200000"
```

Every seed is reproducible on its own: a failure prints the seed, and
`./build/fuzz --seed N` replays exactly that campaign.

`../vect2.cpp` is always compiled with the exam's own flags
(`-Wall -Wextra -Werror -std=c++98`). Only the test code is allowed to need
anything more.

## What each layer is for

### `test_units.cpp` — deterministic edge cases

196 checks, grouped by operator. The parts worth knowing about:

- **Reference identity, not just value.** `+=`, `-=`, `*=`, `=`, `++v` and
  `--v` must return `*this`, so the tests assert `&(v += w) == &v`. An
  implementation returning a copy still passes a naive value check but breaks
  `v2 += v2 += v3` and `(c += a) += b`.
- **Postfix returns a copy.** `(q++)++` must leave `q` incremented exactly
  once — that fails immediately if postfix returns `*this`.
- **Operands survive.** `a + b`, `a - b` and `-a` must not touch their
  operands. This is the bug in `training/v1`, where `operator+` was written as
  `return (*this += other);`.
- **Aliasing.** `s += s`, `s -= s`, `a += a += b`, `(c += a) += b`,
  `v = v - v + v`, `s = s` and `s = alias_of_s`.
- **The output format is checked as an equivalence**, the way the subject
  states it: `os << v` must produce the same text as
  `os << "{" << v[0] << ", " << v[1] << "}"`, rather than hard-coding `{x, y}`
  in one place only.
- **Boundaries**: `INT_MAX` / `INT_MIN` in every case where the result is
  still well-defined (`hi - hi`, `hi + lo`, `-INT_MAX`, `* 0`, `* 1`,
  `++` up to `INT_MAX`, `--` down to `INT_MIN`). Cases that would overflow a
  signed int are deliberately absent: that is undefined behaviour in the
  caller's expression, not a defect in `vect2`.

### `test_fuzz.cpp` — differential fuzzing

Two campaigns, both comparing `vect2` against a reference model that holds the
same numbers in 64-bit `long`.

1. **Stateful.** Six live `vect2` objects, each shadowed by a model. Each step
   picks one of 25 operations and random operands — deliberately allowing the
   operands to alias, so `v[a] += v[b]` becomes `v[a] += v[a]` a sixth of the
   time. After every step the *entire* state is read back twice, once through
   `operator[]` and once through `operator<<`, and compared to the model.
2. **Expression trees.** Random nested expressions like
   `-(3 * ({1, 2} + {4, 5}) - {7, 8}) * -2`, evaluated once with `vect2` and
   once with the model. This reaches operator combinations a flat list of
   operations never will.

The model is computed *first*. If a result would not fit in an `int` the
operation is skipped rather than performed, because signed overflow is
undefined behaviour — the fuzzer must not manufacture UB and then blame
`vect2` for it. A typical run applies ~184k operations and skips ~15k.

```sh
./build/fuzz --seed 7 --steps 500000 --trees 50000
./build/fuzz --random           # seed from the clock
```

### `mutation.sh` — testing the tests

A passing suite proves nothing until you know it can fail. This injects 25
one-line bugs into copies of `vect2.cpp` / `vect2.hpp` — swapped components,
postfix returning the new value, `!=` aliased to `==`, `operator<<` printing
`(x, y)`, the free `n * v` dropping its scalar, and so on — and requires every
one of them to be caught. All 25 are currently killed, 21 by the tests and 4
at compile time.

The mutants are copied *together with the test sources* into a scratch
directory, because the tests include `"../vect2.hpp"` relative to their own
location: compiling them in place would silently pick up the pristine header
and miss every mutation living in it.

### `compile_checks.sh`

- **Warning matrix**: g++ and clang++, each at `c++98/03/11/17`, with the exam
  flags plus `-pedantic-errors -Wshadow -Wold-style-cast -Wconversion
  -Wsign-conversion -Wcast-qual -Weffc++`.
- **const-correctness as negative tests**: `c[0] = 5`, `++c`, `c += ...`,
  `c = ...` on a `const vect2` must all be *rejected*, while every operation
  the subject performs on `v3` must still be accepted.
- **ODR**: `operator<<` and the free `n * v` are defined inside the class body,
  which makes them implicitly `inline`. Including the header from three
  translation units and linking them proves it.
- The graded files are checked to stand alone: `vect2.cpp` defines no `main`
  (the grader supplies its own), the header compiles with nothing included
  before it, and it survives being included twice.

### `expected_subject.txt`

The output of the subject's `main`. Each line was verified by hand against the
comments the subject itself puts on those statements (`// 2, 3`, `// 3, 6`,
`// 20, 40`, …) rather than being captured from the implementation, so the diff
is a real check and not a snapshot of whatever the code happened to print.

Note that the subject's comments describe the *state after* each line, while
`std::cout << v4++` prints the value *before* the increment — which is why the
expected output shows `{1, 2}` on the line commented `// 2, 3`.
