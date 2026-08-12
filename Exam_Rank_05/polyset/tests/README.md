# polyset test harness

Nothing in here is graded. The exam wants six files —
`searchable_array_bag.{cpp,hpp}`, `searchable_tree_bag.{cpp,hpp}` and
`set.{cpp,hpp}` — and this directory exists to prove those six are right.

```sh
make test        # everything below, in order
make units       # deterministic edge cases      (ARGS=-v to see each check)
make fuzz        # differential fuzzing
make subject     # run ../main.cpp, diff against expected_subject.txt
make compile     # warning matrix, canonical form, const-correctness, ODR
make mutants     # prove the suite actually catches bugs
make sanitize    # ASan + UBSan
make valgrind    # leaks and invalid accesses
make soak        # 60 seeds   (SEEDS=300 make soak for more)
```

`make soak` takes two knobs, both read from the environment as well as the
command line, so the same target serves a quick regression and an overnight
run:

```sh
make soak SEEDS=500
SEEDS=500 make soak                # same thing, e.g. from ../../run_all.sh
make soak SEEDS=200 FUZZARGS="--bags 40000 --sets 40000 --views 30000"
```

Every seed is reproducible on its own: a failure prints the seed, and
`./build/fuzz --seed N` replays exactly that campaign.

The graded files are always compiled with the exam's own flags
(`-Wall -Wextra -Werror -std=c++98`). Only the test code is allowed to need
anything more.

## What is graded and what is not

`bag.hpp`, `searchable_bag.hpp`, `array_bag.{cpp,hpp}` and
`tree_bag.{cpp,hpp}` are **provided by the subject**. The grader compiles its
own copies of them, so this harness never modifies them and never fails on a
defect that originates inside them. Two such defects exist and are worth
knowing about — see *Provided-file defects* at the bottom.

## What each layer is for

### `test_units.cpp` — deterministic edge cases

186 checks. `print()` writes to `std::cout`, so the tests steal
`std::cout.rdbuf()` for the duration of the call and compare the text. The
parts worth knowing about:

- **The two bags differ in exactly one observable way.** An array bag prints
  in insertion order; a tree bag prints sorted. Everything else — duplicates
  kept, `has()` semantics, canonical form — is asserted identically for both,
  via a template run twice.
- **Orthodox canonical form under a hostile base.** Both provided bases
  implement `operator=` as *clear, then copy from the source*, which wipes the
  object when the source **is** the object. `s = s` and `s = alias_of_s` are
  checked on both bags; without a `this != &o` guard in the graded file they
  empty the bag. That guard is the one real bug this suite found in the
  original solution.
- **Deep copies, in both directions.** After `Bag b(a)`, inserting into `b`
  must not reach `a`, inserting into `a` must not reach `b`, and clearing `a`
  must leave `b` intact.
- **`has()` on the shapes that break a BST walk.** Ascending inserts build a
  right-leaning chain and descending inserts a left-leaning one; both ends of
  both chains must be findable, and values just past either end must not be.
  Duplicates go to the right of an equal node, so `has()` must not stop at the
  first node that is merely `>=` the target.
- **The virtual base is real.** `bag` is inherited virtually by both
  `array_bag`/`tree_bag` and `searchable_bag`, so there is one shared
  sub-object. The tests reach the same object through `bag*`, through
  `searchable_bag*`, and cross-cast between them with `dynamic_cast`.
- **`set` is a view, not an owner.** Two sets over one bag must not
  double-insert; a value written straight into the bag behind their backs
  becomes visible through both; a copied set wraps the *same* bag, so an
  insert through the copy shows up in the original.
- **Boundaries**: `INT_MIN`, `-1`, `0`, `1`, `INT_MAX` in both bags, including
  the tree's sort order across the whole `int` range.

### `test_fuzz.cpp` — differential fuzzing

Every bag is shadowed by a `std::vector<int>` holding the same values in
insertion order. The expected `print()` output falls straight out of that
vector: as written for an array bag, sorted for a tree bag. Three campaigns:

1. **Bags.** Five live objects per implementation, shadowed by models.
   `insert`, `insert(array, n)`, `clear`, copy-construct, copy-assign,
   assign-a-temporary and insert-through-`searchable_bag&`. Operands are drawn
   independently so `v[a] = v[b]` becomes `v[a] = v[a]` a fifth of the time.
   After every step the whole state is compared, plus 30 `has()` probes —
   values that *are* in the bag, values one away from one that is, and random
   values.
2. **Sets.** Ten sets, five over array bags and five over tree bags, with the
   model applying one extra rule: an insert whose value is already present is
   dropped. `get_bag()` is compared against the set's own `print()` each step.
3. **Shared views.** Five sets over two bags, mixing inserts made through a
   view (deduplicated) with inserts made straight into the bag (not
   deduplicated, and the views must show them).

Sizes are capped at 600 values: `tree_bag`'s destructor recurses, so an
unbounded run of ascending inserts would overflow the stack rather than find
a bug.

```sh
./build/fuzz --seed 7 --bags 50000 --sets 50000 --views 20000
./build/fuzz --random            # seed from the clock
```

### `mutation.sh` — testing the tests

A passing suite proves nothing until you know it can fail. This injects 38
one-line bugs into copies of the six graded files — a tree search that
descends the wrong branch, an `operator=` that loses its self-assignment
guard, a `set::insert` that forgets to deduplicate, a bulk insert that skips
the dedup, a `has()` that is no longer `const` — and requires every one of
them to be caught. All 38 are currently killed: 29 by the tests, 8 at compile
time, 1 by a crash.

The provided files are copied in unmutated beside each mutant, together with
the test sources, because the tests include `"../searchable_array_bag.hpp"`
relative to their own location: compiling them in place would silently pick up
the pristine headers.

### `compile_checks.sh`

- **The submission, rebuilt in isolation.** The subject collects exactly six
  files. They are copied into an empty directory next to *pristine* copies of
  the seven the subject provides, built with
  `c++ -Wall -Wextra -Werror -std=c++98 *.cpp`, and run — which proves the six
  are a complete solution and that none of them depends on a local edit to a
  provided file.
- **Warning matrix**: g++ and clang++, each at `c++98/03/11/17`, over the three
  graded `.cpp` files, with the exam flags plus `-pedantic-errors -Wshadow
  -Wold-style-cast -Wconversion -Wsign-conversion -Wcast-qual`. `-Weffc++` and
  `-Wnon-virtual-dtor` are deliberately absent: both fire inside the provided
  headers, which the graded files cannot change.
- **Orthodox canonical form as positive tests**: default ctor, copy ctor, copy
  assignment and destructor for both bags, plus `a = b = c` and
  `Bag& r = (a = b)` to pin down that assignment returns a reference.
- **const-correctness as negative tests**: `insert` and `clear` on a `const`
  bag or a `const set` must be *rejected*, while `print`, `has` and `get_bag`
  must still be accepted on one.
- **The inheritance graph**: neither class may be left abstract; both must work
  through `bag*` and through `searchable_bag*`; both `insert` overloads must
  stay visible in the derived class (omitting a `using` is the classic way to
  hide `insert(int*, int)`); and a linked program checks that the two
  inheritance paths reach the **same** `bag` sub-object, which is what virtual
  inheritance buys.
- **ODR**: all three graded classes are defined entirely inside their headers,
  so every member is implicitly `inline`. Including them from four translation
  units and linking proves it.
- Each graded `.cpp` is checked to compile alone, define no `main`, and have a
  header that is self-contained and safe to include twice.

### `expected_subject.txt`

The output of `../main.cpp` run as `./subject 5 3 5 1`. Each line was worked
out by hand from the source — including that `st` and `sa` wrap **the same
bag**, so the four arguments `5 3 5 1` deduplicate down to `5 3 1` once,
printed three times — rather than captured from the implementation.

Note that every value line ends with a **trailing space**: the provided
`print()` writes `value << " "` per element. The diff is exact, so that space
is part of the expected output.

## Provided-file defects

Neither can be fixed from the six graded files. Both are reported by
`make compile` as `[--]` notes rather than failures.

1. **`array_bag.cpp` frees a `new int[...]` array with plain `delete`**, in the
   destructor, in `insert()` and in `clear()`. This is undefined behaviour that
   both ASan and valgrind flag. `make valgrind` loads `provided.supp`, which
   suppresses exactly that pattern *inside `array_bag` frames only*, and
   `make sanitize` sets `ASAN_OPTIONS=alloc_dealloc_mismatch=0`. The same
   mistake made anywhere in the graded code is still reported.
2. **`bag` has no virtual destructor**, so `delete` through a `bag*` or a
   `searchable_bag*` is undefined. The subject's `main` allocates two bags with
   `new` and never deletes them, so it never hits this; the tests use automatic
   storage for the same reason.

## A note on `set` and canonical form

`set` has a copy constructor, a copy assignment operator and a destructor, but
**no default constructor**. That is deliberate: a `set` is a view onto a
`searchable_bag` supplied at construction, and a default-constructed one would
hold a null bag that makes every other member crash. The subject's `main` never
default-constructs a set. If your evaluator insists on all four members, the
honest fix is to give `set` ownership of a bag rather than a pointer to
someone else's — which changes what `get_bag()` means and is a different
design, not a one-line addition.
