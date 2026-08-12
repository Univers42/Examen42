#!/usr/bin/env python3
"""Deterministic edge cases for life.

Every expectation in this file is written out by hand from ../subject.txt --
none of it comes from the oracle or from running the program. That is what
makes this layer an independent check on test_fuzz.py, which does use the
oracle.

Boards are written with '.' for a dead cell so the trailing spaces stay
visible in the source; grid() turns them back into the spaces the program has
to print.

usage: python3 test_units.py [path/to/life] [-v]
"""
import os
import subprocess
import sys

LIFE = sys.argv[1] if len(sys.argv) > 1 else "../life"
VERBOSE = "-v" in sys.argv[2:]

GREEN, RED, CYAN, OFF = "\033[32m", "\033[1;31m", "\033[36m", "\033[0m"

passed = failed = 0


def report(ok, label, detail=""):
    global passed, failed
    if ok:
        passed += 1
        if VERBOSE:
            print("  %s[ok]%s %s" % (GREEN, OFF, label))
    else:
        failed += 1
        print("  %s[KO]%s %s" % (RED, OFF, label))
        if detail:
            print(detail)


def section(title):
    print("\n%s== %s ==%s" % (CYAN, title, OFF))


def grid(*rows):
    """'.' means dead, 'O' means alive; the program prints a space for dead."""
    return "".join(r.replace(".", " ") + "\n" for r in rows)


def run(args, stdin=""):
    p = subprocess.run([LIFE] + [str(a) for a in args],
                       input=stdin, capture_output=True, text=True)
    return p.stdout, p.stderr, p.returncode


def check(label, w, h, k, commands, want, rc=0):
    got, err, got_rc = run([w, h, k], commands)
    ok = (got, err, got_rc) == (want, "", rc)
    detail = ""
    if not ok:
        detail = ("        ./life %s %s %s  <<< %r\n"
                  "        expected (rc %d):\n%s"
                  "        got (rc %d):\n%s"
                  % (w, h, k, commands, rc,
                     "".join("          |%s|\n" % l for l in want.split("\n")[:-1]),
                     got_rc,
                     "".join("          |%s|\n" % l for l in got.split("\n")[:-1])))
        if err:
            detail += "        stderr: %r\n" % err
    report(ok, label, detail)


# ---------------------------------------------------------------------------

def testSubjectExamples():
    section("The subject's own examples")

    # $> echo 'sdxddssaaww' | ./a.out 5 5 0
    check("example 1: a ring drawn on a 5x5 board", 5, 5, 0, "sdxddssaaww\n",
          grid(".....",
               ".OOO.",
               ".O.O.",
               ".OOO.",
               "....."))

    # $> echo 'sdxssdswdxddddsxaadwxwdxwaa' | ./a.out 10 6 0
    check("example 2: two shapes on a 10x6 board", 10, 6, 0,
          "sdxssdswdxddddsxaadwxwdxwaa\n",
          grid("..........",
               ".O...OOO..",
               ".O.....O..",
               ".OOO..O...",
               "..O..OOO..",
               ".........."))

    # $> echo 'dxss' | ./a.out 3 3 0 / 1 / 2 -- a blinker, period 2
    check("example 3: a vertical blinker, 0 iterations", 3, 3, 0, "dxss\n",
          grid(".O.", ".O.", ".O."))
    check("example 4: the same blinker after 1 iteration", 3, 3, 1, "dxss\n",
          grid("...", "OOO", "..."))
    check("example 5: and back again after 2", 3, 3, 2, "dxss\n",
          grid(".O.", ".O.", ".O."))

    # The period is 2, so every even count comes back to the start.
    for k in (4, 6, 20):
        check("the blinker after %d iterations" % k, 3, 3, k, "dxss\n",
              grid(".O.", ".O.", ".O."))
    for k in (3, 5, 21):
        check("the blinker after %d iterations" % k, 3, 3, k, "dxss\n",
              grid("...", "OOO", "..."))


def testPen():
    section("The pen")

    check("no commands leaves an empty board", 3, 3, 0, "",
          grid("...", "...", "..."))

    # "Imagine a pen starting in the top left corner of the board."
    check("lowering the pen draws the cell it is already on", 3, 3, 0, "x",
          grid("O..", "...", "..."))

    check("moving with the pen up draws nothing", 3, 3, 0, "dds",
          grid("...", "...", "..."))

    check("the pen draws each cell it moves onto", 3, 3, 0, "xdd",
          grid("OOO", "...", "..."))

    check("lifting the pen stops the drawing", 3, 3, 0, "xdxd",
          grid("OO.", "...", "..."))

    check("and lowering it again resumes, on the spot", 3, 3, 0, "xdxdx",
          grid("OOO", "...", "..."))

    check("toggling twice on one cell changes nothing", 3, 3, 0, "xx",
          grid("O..", "...", "..."))

    check("toggling three times leaves the pen down", 3, 3, 0, "xxxd",
          grid("OO.", "...", "..."))

    # d, s, w and a in turn: right, down, up, left, back to the start.
    check("all four directions", 3, 3, 0, "xdswa",
          grid("OO.", ".O.", "..."))

    check("a full lap around a 3x3 board", 3, 3, 0, "xddssaaww",
          grid("OOO", "O.O", "OOO"))

    section("The pen outside the board")

    # "each cell outside of the array will be considered dead" -- and the pen
    # keeps its position while it is out there, so it can come back.
    check("going off the top and coming back", 3, 3, 0, "xwwss",
          grid("O..", "...", "..."))

    check("going off the left and coming back", 3, 3, 0, "xaadd",
          grid("O..", "...", "..."))

    check("going off the right and coming back", 3, 3, 0, "xddddaaaa",
          grid("OOO", "...", "..."))

    check("going off the bottom and coming back", 3, 3, 0, "xssssswwwww",
          grid("O..", "O..", "O.."))

    check("a wide excursion returns to the right column", 3, 3, 0,
          "xdddddddddaaaaaaaaa",
          grid("OOO", "...", "..."))

    # The pen is lowered three cells above and three cells left of the board;
    # it draws nothing until the very last 'd' brings it back onto (0, 0).
    check("drawing only after re-entering", 3, 3, 0, "wwwaaaxsssddd",
          grid("O..", "...", "..."))

    section("Characters that are not commands")

    # The subject lists w a s d and x. Anything else must change nothing.
    check("a trailing newline is not a command", 3, 3, 0, "xd\n",
          grid("OO.", "...", "..."))
    check("letters that are not commands are ignored", 3, 3, 0, "xQdZ",
          grid("OO.", "...", "..."))
    check("digits and punctuation are ignored", 3, 3, 0, "x1d!",
          grid("OO.", "...", "..."))
    check("uppercase commands are not commands", 3, 3, 0, "xDS",
          grid("O..", "...", "..."))
    check("spaces and tabs are ignored", 3, 3, 0, "x d\ts",
          grid("OO.", ".O.", "..."))
    check("a NUL byte is ignored", 3, 3, 0, "x\0d",
          grid("OO.", "...", "..."))


def testRules():
    section("The rules of the game")

    # Fewer than two neighbours: a lone cell dies.
    check("one live cell dies of loneliness", 3, 3, 1, "x",
          grid("...", "...", "..."))
    check("two live cells both die", 3, 3, 1, "xd",
          grid("...", "...", "..."))

    # Exactly three neighbours: a dead cell is born. Three in an L already
    # makes a block on the next generation.
    check("an L of three becomes a block", 4, 4, 1, "xds",
          grid("OO..", "OO..", "....", "...."))

    # A block is a still life: three neighbours each, forever.
    for k in (0, 1, 2, 7):
        check("a 2x2 block is stable after %d iterations" % k, 4, 4, k, "xdsa",
              grid("OO..", "OO..", "....", "...."))

    # A blinker against the left edge: the outside counts as dead, so it does
    # not oscillate the way a centred one does.
    check("a blinker on the edge, 0 iterations", 3, 3, 0, "xss",
          grid("O..", "O..", "O.."))
    check("a blinker on the edge dies back to two cells", 3, 3, 1, "xss",
          grid("...", "OO.", "..."))
    check("and those two die of loneliness", 3, 3, 2, "xss",
          grid("...", "...", "..."))

    # A hollow ring: the eight cells around a dead centre.
    check("a hollow 3x3 ring, 0 iterations", 5, 5, 0, "xddssaaww",
          grid("OOO..", "O.O..", "OOO..", ".....", "....."))

    # Overcrowding, on a solid 3x3 block. The centre has eight neighbours and
    # dies; the four edge midpoints have five and die; the four corners have
    # three and live; and the two cells that see exactly three of the block --
    # (3,1) to its right and (1,3) below it -- are born.
    check("a solid 3x3 block, 0 iterations", 5, 5, 0, "xddsaasdd",
          grid("OOO..", "OOO..", "OOO..", ".....", "....."))
    check("a solid 3x3 block thins out after 1 iteration", 5, 5, 1, "xddsaasdd",
          grid("O.O..", "...O.", "O.O..", ".O...", "....."))

    # An empty board stays empty however long it runs.
    check("an empty board stays empty", 4, 4, 10, "",
          grid("....", "....", "....", "...."))

    # A board one cell wide still has vertical neighbours: the two middle
    # cells of a run of four keep exactly two each and survive, while the two
    # ends keep one and die.
    check("a 1-wide run of four keeps its middle", 1, 5, 1, "xsss",
          grid(".", "O", "O", ".", "."))
    check("a 1-wide pair dies of loneliness", 1, 5, 1, "xs",
          grid(".", ".", ".", ".", "."))


def testGlider():
    section("A glider")

    # dxxdsxxsxaa draws
    #     .O.
    #     ..O
    #     OOO
    # in the top-left corner: 'd' then 'x' marks (1,0); 'x d s x' marks (2,1);
    # 'x s x' marks (2,2) and the two 'a's trail back over (1,2) and (0,2).
    glider = "dxxdsxxsxaa"

    check("the glider as drawn", 10, 10, 0, glider,
          grid(".O........",
               "..O.......",
               "OOO.......",
               "..........",
               "..........",
               "..........",
               "..........",
               "..........",
               "..........",
               ".........."))

    # A glider repeats its own shape one cell right and one cell down every
    # four generations. That is a property of the pattern, not of this
    # implementation, so it is a genuine outside check.
    check("after 4 generations it has moved one cell down and right",
          10, 10, 4, glider,
          grid("..........",
               "..O.......",
               "...O......",
               ".OOO......",
               "..........",
               "..........",
               "..........",
               "..........",
               "..........",
               ".........."))

    check("after 8 generations it has moved two", 10, 10, 8, glider,
          grid("..........",
               "..........",
               "...O......",
               "....O.....",
               "..OOO.....",
               "..........",
               "..........",
               "..........",
               "..........",
               ".........."))


def testShape():
    section("Board shape and output format")

    check("a 1x1 board, empty", 1, 1, 0, "", grid("."))
    check("a 1x1 board, drawn", 1, 1, 0, "x", grid("O"))
    check("a 1x1 board, one iteration kills it", 1, 1, 1, "x", grid("."))

    check("a wide board keeps its width", 6, 1, 0, "xdd",
          grid("OOO..."))
    check("a tall board keeps its height", 1, 6, 0, "xss",
          grid("O", "O", "O", ".", ".", "."))

    # Every row ends with a newline, including the last one, and no row is
    # trimmed: a board of dead cells still prints its full width in spaces.
    out, _, _ = run([4, 3, 0], "")
    report(out == "    \n    \n    \n", "dead cells are printed as spaces, not skipped",
           "        got %r" % out)
    report(out.count("\n") == 3, "one newline per row")
    report(all(len(l) == 4 for l in out.split("\n")[:-1]), "every row is 4 wide")

    # A 40x25 board: the total size must be exactly rows * (cols + 1).
    out, _, _ = run([40, 25, 0], "x")
    report(len(out) == 25 * 41, "a 40x25 board prints 25 * 41 characters",
           "        got %d" % len(out))
    report(out[0] == "O" and out.count("O") == 1, "with exactly one live cell, at the start")


def testArguments():
    section("Arguments")

    for args in ([], [5], [5, 5], [5, 5, 0, 0], [5, 5, 0, 0, 0]):
        out, err, rc = run(args, "x")
        report(out == "" and rc != 0,
               "%d argument(s) is rejected with no output" % len(args),
               "        got out=%r rc=%d" % (out, rc))

    # atoi of something that is not a number is 0, and a board with no cell
    # has nothing to print.
    for w, h in (("abc", 5), (5, "abc"), (0, 5), (5, 0), (0, 0), (-3, 5), (5, -3)):
        out, err, rc = run([w, h, 0], "x")
        report(out == "", "./life %s %s 0 prints nothing" % (w, h),
               "        got %r" % out)

    # A negative or non-numeric iteration count means no iteration at all.
    check("a negative iteration count runs no generation", 3, 3, -5, "x",
          grid("O..", "...", "..."))
    out, _, _ = run([3, 3, "abc"], "x")
    report(out == grid("O..", "...", "..."), "a non-numeric iteration count means zero",
           "        got %r" % out)

    # atoi stops at the first non-digit, so trailing junk is ignored.
    out, _, _ = run(["3x", "3y", "0z"], "x")
    report(out == grid("O..", "...", "..."), "atoi ignores trailing junk in the arguments",
           "        got %r" % out)

    # Leading spaces and a plus sign are part of what atoi accepts.
    out, _, _ = run([" 3", "+3", 0], "x")
    report(out == grid("O..", "...", "..."), "atoi accepts a leading space and a plus",
           "        got %r" % out)

    section("Input")

    check("empty stdin on a board that stays empty", 3, 3, 0, "",
          grid("...", "...", "..."))
    check("a command stream with no trailing newline", 3, 3, 0, "xd",
          grid("OO.", "...", "..."))
    # 1000 commands, most of them off the board: the pen steps right-down
    # diagonally and leaves after four moves, so only the first five cells it
    # touched are drawn.
    check("a very long command stream", 3, 3, 0, "x" + "ds" * 500,
          grid("OO.", ".OO", "..O"))

    # Nothing is ever written to stderr.
    _, err, _ = run([5, 5, 3], "xdsdsa")
    report(err == "", "nothing is written to stderr", "        got %r" % err)


def main():
    if not os.path.exists(LIFE):
        print("%s[KO]%s no such binary: %s" % (RED, OFF, LIFE))
        return 1

    testSubjectExamples()
    testPen()
    testRules()
    testGlider()
    testShape()
    testArguments()

    total = passed + failed
    print("\n-----------------------------")
    if failed == 0:
        print("%s%d/%d checks passed%s" % (GREEN, passed, total, OFF))
        return 0
    print("%s%d/%d checks passed (%d failed)%s" % (RED, passed, total, failed, OFF))
    return 1


if __name__ == "__main__":
    sys.exit(main())
