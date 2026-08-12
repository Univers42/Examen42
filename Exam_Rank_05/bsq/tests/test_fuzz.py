#!/usr/bin/env python3
"""Differential fuzzing for bsq.

Random maps -- valid ones, deliberately corrupted ones, and pure noise -- are
run through the program and through oracle.py, and the two answers must match
exactly on both stdout and stderr.

Maps are fed in batches as several arguments to one process, which is both
much faster than one process per map and a heavy test of the multi-map
requirement: the concatenation of the batch's answers must equal the
concatenation of the oracle's, with each "map error" landing on stderr in the
right order.

usage: python3 test_fuzz.py [path/to/bsq] [--seed N] [--maps N] [--batch N]
                            [--big N] [--sep none|blank] [--random]
"""
import os
import random
import string
import subprocess
import sys
import tempfile
import time

import oracle

GREEN, RED, CYAN, OFF = "\033[32m", "\033[1;31m", "\033[36m", "\033[0m"

# Printable characters a map is allowed to use. Space is in there on purpose:
# the first line is read by position, so " " is a legal map character and the
# usual "split on whitespace" parser gets it wrong.
CHARS = string.digits + string.ascii_letters + string.punctuation + " "

failures = 0

# See README.md: the subject's one genuine ambiguity. "" means consecutive
# maps' answers run straight together (every line already ends in a newline);
# "\n" means an extra blank line separates them on stdout.
SEP = ""


def fail(what, want, got):
    global failures
    failures += 1
    if failures <= 10:
        print("  %s[KO]%s %s" % (RED, OFF, what))
        print("        expected: %r" % (want,))
        print("        got:      %r" % (got,))


# --- map generation --------------------------------------------------------

def random_valid(rng, maxdim=14):
    """A well-formed map. Densities are drawn per map, so the corpus contains
    boards that are all empty, all obstacles, and everything between."""
    e, o, f = rng.sample(CHARS, 3)
    rows = rng.randint(1, maxdim)
    cols = rng.randint(1, maxdim)
    shape = rng.randrange(8)

    if shape == 0:                                    # all empty
        grid = [[e] * cols for _ in range(rows)]
    elif shape == 1:                                  # all obstacles
        grid = [[o] * cols for _ in range(rows)]
    elif shape == 2:                                  # exactly one obstacle
        grid = [[e] * cols for _ in range(rows)]
        grid[rng.randrange(rows)][rng.randrange(cols)] = o
    elif shape == 3:                                  # checkerboard
        grid = [[e if (i + j) % 2 == 0 else o for j in range(cols)] for i in range(rows)]
    elif shape == 4:                                  # diagonal wall
        grid = [[o if i == j else e for j in range(cols)] for i in range(rows)]
    elif shape == 5:                                  # obstacle border
        grid = [[o if i in (0, rows - 1) or j in (0, cols - 1) else e
                 for j in range(cols)] for i in range(rows)]
    elif shape == 6:                                  # two equal-sized squares
        # Tie-break bait: identical empty blocks placed so that only the
        # top-then-left rule tells them apart.
        rows = cols = rng.randint(4, maxdim)
        grid = [[o] * cols for _ in range(rows)]
        side = rng.randint(1, max(1, rows // 2))
        for i in range(side):
            for j in range(side):
                grid[i][j] = e
                grid[rows - side + i][cols - side + j] = e
                if side < rows:
                    grid[i][cols - side + j] = e
    else:                                             # random density
        p = rng.random()
        grid = [[o if rng.random() < p else e for _ in range(cols)] for _ in range(rows)]

    body = "".join("".join(r) + "\n" for r in grid)
    return "%d %s %s %s\n%s" % (rows, e, o, f, body)


MUTATIONS = [
    "drop_final_newline", "extra_line", "drop_line", "grow_line", "shrink_line",
    "bad_char", "full_char", "dup_chars", "wrong_count", "blank_first",
    "tab_separator", "double_space", "drop_char", "extra_char", "zero_count",
    "huge_count", "empty_line", "leading_blank", "cr_in_line", "truncate",
]


def corrupt(rng, data):
    """Break a valid map in one specific way. The oracle decides whether the
    result is actually invalid -- some of these stay legal, which is exactly
    the interesting case."""
    kind = rng.choice(MUTATIONS)
    lines = data.split("\n")[:-1]
    head, body = lines[0], lines[1:]

    if kind == "drop_final_newline":
        return data[:-1]
    if kind == "extra_line" and body:
        return data + body[-1] + "\n"
    if kind == "drop_line" and len(body) > 1:
        del body[rng.randrange(len(body))]
    elif kind == "grow_line" and body:
        i = rng.randrange(len(body))
        body[i] += body[i][-1] if body[i] else "."
    elif kind == "shrink_line" and body:
        i = rng.randrange(len(body))
        body[i] = body[i][:-1]
    elif kind == "bad_char" and body:
        i = rng.randrange(len(body))
        j = rng.randrange(len(body[i])) if body[i] else 0
        c = rng.choice(CHARS + "\t")
        body[i] = body[i][:j] + c + body[i][j + 1:]
    elif kind == "full_char" and body:
        i = rng.randrange(len(body))
        if body[i]:
            j = rng.randrange(len(body[i]))
            body[i] = body[i][:j] + head[-1] + body[i][j + 1:]
    elif kind == "dup_chars":
        n = 0
        while n < len(head) and head[n].isdigit():
            n += 1
        which = rng.randrange(3)
        src, dst = [(1, 3), (1, 5), (3, 5)][which]
        head = head[:n + dst] + head[n + src] + head[n + dst + 1:]
    elif kind == "wrong_count":
        n = 0
        while n < len(head) and head[n].isdigit():
            n += 1
        head = str(max(0, int(head[:n]) + rng.choice([-1, 1, 2]))) + head[n:]
    elif kind == "blank_first":
        return "\n" + data
    elif kind == "tab_separator":
        head = head.replace(" ", "\t", 1)
    elif kind == "double_space":
        n = 0
        while n < len(head) and head[n].isdigit():
            n += 1
        head = head[:n] + " " + head[n:]
    elif kind == "drop_char":
        head = head[:-2]
    elif kind == "extra_char":
        head = head + " " + rng.choice(CHARS)
    elif kind == "zero_count":
        n = 0
        while n < len(head) and head[n].isdigit():
            n += 1
        head = "0" + head[n:]
    elif kind == "huge_count":
        n = 0
        while n < len(head) and head[n].isdigit():
            n += 1
        head = "9999999999" + head[n:]
    elif kind == "empty_line" and body:
        body[rng.randrange(len(body))] = ""
    elif kind == "leading_blank":
        head = " " + head
    elif kind == "cr_in_line" and body:
        i = rng.randrange(len(body))
        body[i] = body[i] + "\r"
    elif kind == "truncate":
        return data[:rng.randrange(len(data) + 1)]

    return "".join(l + "\n" for l in [head] + body)


def noise(rng):
    """Pure garbage, to make sure nothing crashes on input that was never a map."""
    n = rng.randrange(40)
    alphabet = CHARS + "\n\n\n\t\r"
    return "".join(rng.choice(alphabet) for _ in range(n))


# --- running ---------------------------------------------------------------

def run_batch(bsq, maps, workdir):
    """Write each map to its own file, run one process over all of them, and
    compare against the concatenated oracle answers."""
    paths, chunks, wants_err = [], [], []
    for k, data in enumerate(maps):
        p = os.path.join(workdir, "m%03d.map" % k)
        with open(p, "w") as fh:
            fh.write(data)
        paths.append(p)
        try:
            solved = oracle.solve(data, cross_check=len(data) < 4000)
        except AssertionError as exc:
            fail("the oracle contradicts itself", str(exc), "")
            return
        # An invalid map contributes nothing to stdout, but it still takes
        # part in the separation -- which is what makes SEP observable.
        chunks.append(solved if solved is not None else "")
        if solved is None:
            wants_err.append("map error\n")

    proc = subprocess.run([bsq] + paths, capture_output=True, text=True)
    want_out, want_err = SEP.join(chunks), "".join(wants_err)

    if proc.stdout != want_out or proc.stderr != want_err:
        # Narrow it down to the single map that differs, so the report is
        # readable instead of being a wall of concatenated boards.
        for data in maps:
            one = subprocess.run([bsq], input=data, capture_output=True, text=True)
            solved = oracle.solve(data)
            w_out, w_err = (solved or ""), ("" if solved else "map error\n")
            if (one.stdout, one.stderr) != (w_out, w_err):
                fail("map %r" % (data,), (w_out, w_err), (one.stdout, one.stderr))
                return
        fail("a batch of %d maps differs, but every map matches on its own"
             % len(maps), (want_out, want_err), (proc.stdout, proc.stderr))


def main():
    args = sys.argv[1:]
    bsq = "../solution/bsq"
    seed, nmaps, batch, big = 1, 4000, 25, 40

    i = 0
    while i < len(args):
        a = args[i]
        if a == "--seed":
            seed = int(args[i + 1]); i += 2
        elif a == "--maps":
            nmaps = int(args[i + 1]); i += 2
        elif a == "--batch":
            batch = int(args[i + 1]); i += 2
        elif a == "--big":
            big = int(args[i + 1]); i += 2
        elif a == "--sep":
            globals()["SEP"] = "\n" if args[i + 1] == "blank" else ""
            i += 2
        elif a == "--random":
            seed = int(time.time()); i += 1
        elif a.startswith("--"):
            print(__doc__)
            return 1
        else:
            bsq = a; i += 1

    if not os.path.exists(bsq):
        print("%s[KO]%s no such binary: %s" % (RED, OFF, bsq))
        return 1

    rng = random.Random(seed)
    bsq = os.path.abspath(bsq)

    print("%s== differential fuzz ==%s  seed %d, %d maps in batches of %d"
          % (CYAN, OFF, seed, nmaps, batch))

    valid = corrupted = garbage = 0
    with tempfile.TemporaryDirectory() as work:
        maps = []
        for _ in range(nmaps):
            roll = rng.random()
            if roll < 0.55:
                data = random_valid(rng); valid += 1
            elif roll < 0.90:
                data = corrupt(rng, random_valid(rng)); corrupted += 1
            else:
                data = noise(rng); garbage += 1
            maps.append(data)
            if len(maps) == batch:
                run_batch(bsq, maps, work)
                maps = []
                if failures:
                    break
        if maps and not failures:
            run_batch(bsq, maps, work)

    if failures == 0:
        print("  %s[ok]%s %d well-formed, %d corrupted, %d garbage -- all agree"
              % (GREEN, OFF, valid, corrupted, garbage))

    # A separate pass on boards too large for the brute-force cross-check,
    # where only the O(n^2) reference runs.
    print("%s== larger boards ==%s  %d maps up to 120x120" % (CYAN, OFF, big))
    with tempfile.TemporaryDirectory() as work:
        for k in range(big):
            if failures:
                break
            run_batch(bsq, [random_valid(rng, maxdim=120)], work)
    if failures == 0:
        print("  %s[ok]%s %d larger boards agree" % (GREEN, OFF, big))

    print("\n-----------------------------")
    if failures == 0:
        print("%sfuzzing found no difference from the oracle%s" % (GREEN, OFF))
        return 0
    print("%s%d difference(s) from the oracle%s" % (RED, failures, OFF))
    return 1


if __name__ == "__main__":
    sys.exit(main())
