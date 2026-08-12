#!/usr/bin/env python3
"""Differential fuzzing for life.

Random boards, random iteration counts and random command streams go through
the program and through oracle.py, and the two answers must match byte for
byte on stdout, with stderr empty.

usage: python3 test_fuzz.py [path/to/life] [--seed N] [--cases N]
                            [--long N] [--random]
"""
import os
import random
import subprocess
import sys
import time

import oracle

GREEN, RED, CYAN, OFF = "\033[32m", "\033[1;31m", "\033[36m", "\033[0m"

# The five real commands, plus the characters that must be ignored: other
# letters, digits, whitespace and punctuation.
COMMANDS = "wasdx"
NOISE = "WASDXqzZ019 \t\n\r.-_/*"

failures = 0


def fail(what, want, got):
    global failures
    failures += 1
    if failures <= 8:
        print("  %s[KO]%s %s" % (RED, OFF, what))
        print("        expected:")
        for line in want.split("\n")[:-1][:14]:
            print("          |%s|" % line)
        print("        got:")
        for line in got.split("\n")[:-1][:14]:
            print("          |%s|" % line)


def random_commands(rng, n, noise_rate):
    """A command stream. `x` is drawn less often than the moves so that runs
    of drawing and runs of travelling are both long enough to matter."""
    out = []
    for _ in range(n):
        if rng.random() < noise_rate:
            out.append(rng.choice(NOISE))
        elif rng.random() < 0.12:
            out.append("x")
        else:
            out.append(rng.choice("wasd"))
    return "".join(out)


def structured_commands(rng, w, h):
    """Streams built to produce shapes the random walk rarely reaches: a full
    sweep of the board, a border, a diagonal, and a pen that spends most of
    its time outside."""
    kind = rng.randrange(5)
    if kind == 0:                                   # fill every cell
        out = ["x"]
        for row in range(h):
            out.append("d" * (w - 1) if row % 2 == 0 else "a" * (w - 1))
            out.append("s")
        return "".join(out)
    if kind == 1:                                   # draw the border
        return "x" + "d" * (w - 1) + "s" * (h - 1) + "a" * (w - 1) + "w" * (h - 1)
    if kind == 2:                                   # a diagonal
        return "x" + "ds" * max(w, h)
    if kind == 3:                                   # mostly outside the board
        return ("w" * rng.randint(1, h + 3) + "a" * rng.randint(1, w + 3) + "x"
                + "".join(rng.choice("wasd") for _ in range(3 * (w + h))))
    # alternating pen: draw one cell in three
    return "".join("xdx" if i % 2 else "dd" for i in range(w + h))


def run(life, w, h, k, commands):
    p = subprocess.run([life, str(w), str(h), str(k)],
                       input=commands.encode(), capture_output=True)
    return p.stdout.decode("latin-1"), p.stderr.decode("latin-1"), p.returncode


def check(life, w, h, k, commands, cross_check):
    want = oracle.simulate(w, h, k, commands, cross_check=cross_check)
    got, err, rc = run(life, w, h, k, commands)
    if got != want or err != "" or rc != 0:
        fail("./life %d %d %d  <<< %r%s"
             % (w, h, k, commands[:60] + ("..." if len(commands) > 60 else ""),
                "" if err == "" else "   (stderr %r, rc %d)" % (err, rc)),
             want, got)
        return False
    return True


def main():
    args = sys.argv[1:]
    life = "../life"
    seed, cases, longer = 1, 3000, 60

    i = 0
    while i < len(args):
        a = args[i]
        if a == "--seed":
            seed = int(args[i + 1]); i += 2
        elif a == "--cases":
            cases = int(args[i + 1]); i += 2
        elif a == "--long":
            longer = int(args[i + 1]); i += 2
        elif a == "--random":
            seed = int(time.time()); i += 1
        elif a.startswith("--"):
            print(__doc__)
            return 1
        else:
            life = a; i += 1

    if not os.path.exists(life):
        print("%s[KO]%s no such binary: %s" % (RED, OFF, life))
        return 1

    rng = random.Random(seed)
    life = os.path.abspath(life)

    # --- small boards, many generations -----------------------------------
    # Small boards are where the edges dominate, and the edge rule ("each cell
    # outside of the array will be considered dead") is the one most often
    # got wrong. Iteration counts go high enough for a pattern to settle.
    print("%s== small boards ==%s  seed %d, %d cases up to 12x12, up to 30 generations"
          % (CYAN, OFF, seed, cases))
    structured = 0
    for _ in range(cases):
        w, h = rng.randint(1, 12), rng.randint(1, 12)
        k = rng.choice([0, 0, 1, 1, 2, 3, rng.randint(0, 30)])
        if rng.random() < 0.25:
            commands = structured_commands(rng, w, h)
            structured += 1
        else:
            commands = random_commands(rng, rng.randint(0, 60), rng.random() * 0.4)
        if not check(life, w, h, k, commands, cross_check=True):
            break
    if failures == 0:
        print("  %s[ok]%s %d cases agree (%d of them from structured pen paths)"
              % (GREEN, OFF, cases, structured))

    # --- degenerate shapes -------------------------------------------------
    print("%s== degenerate boards ==%s  1-wide, 1-tall and 1x1" % (CYAN, OFF))
    n = 0
    for _ in range(200):
        if failures:
            break
        w, h = rng.choice([(1, rng.randint(1, 30)), (rng.randint(1, 30), 1), (1, 1)])
        k = rng.randint(0, 6)
        commands = random_commands(rng, rng.randint(0, 40), 0.2)
        check(life, w, h, k, commands, cross_check=True)
        n += 1
    if failures == 0:
        print("  %s[ok]%s %d degenerate boards agree" % (GREEN, OFF, n))

    # --- larger boards -----------------------------------------------------
    print("%s== larger boards ==%s  %d cases up to 60x40" % (CYAN, OFF, longer))
    for _ in range(longer):
        if failures:
            break
        w, h = rng.randint(20, 60), rng.randint(20, 40)
        k = rng.randint(0, 8)
        commands = random_commands(rng, rng.randint(50, 1500), 0.15)
        check(life, w, h, k, commands, cross_check=False)
    if failures == 0:
        print("  %s[ok]%s %d larger boards agree" % (GREEN, OFF, longer))

    # --- argument shapes ---------------------------------------------------
    # Arguments that are not usable dimensions must produce no output at all,
    # and must never crash.
    print("%s== unusable arguments ==%s" % (CYAN, OFF))
    bad = 0
    for w, h, k in [(0, 5, 0), (5, 0, 0), (0, 0, 0), (-1, 5, 0), (5, -1, 0),
                    (-4, -4, 3), (0, 0, -1), (-1, -1, -1)]:
        got, err, rc = run(life, w, h, k, "xdsxsd")
        if got != "" or err != "" or rc > 128:
            fail("./life %d %d %d must print nothing and not crash (rc %d)"
                 % (w, h, k, rc), "", got)
        else:
            bad += 1
    if failures == 0:
        print("  %s[ok]%s %d unusable argument sets print nothing" % (GREEN, OFF, bad))

    # A negative iteration count is not unusable: the board is still drawn,
    # it just never steps. `while (k-- > 0)` and `for _ in range(max(0, k))`
    # have to agree about that.
    neg = 0
    for _ in range(40):
        if failures:
            break
        w, h = rng.randint(1, 8), rng.randint(1, 8)
        commands = random_commands(rng, rng.randint(0, 30), 0.2)
        want = oracle.simulate(w, h, 0, commands)
        got, err, rc = run(life, w, h, -rng.randint(1, 50), commands)
        if (got, err, rc) != (want, "", 0):
            fail("a negative iteration count must behave like 0", want, got)
        else:
            neg += 1
    if failures == 0:
        print("  %s[ok]%s %d negative iteration counts behave like 0" % (GREEN, OFF, neg))

    print("\n-----------------------------")
    if failures == 0:
        print("%sfuzzing found no difference from the oracle%s" % (GREEN, OFF))
        return 0
    print("%s%d difference(s) from the oracle%s" % (RED, failures, OFF))
    return 1


if __name__ == "__main__":
    sys.exit(main())
