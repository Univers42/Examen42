#!/usr/bin/env python3
"""Deterministic edge cases for bsq.

Every expectation in this file is written out by hand from ../subject.txt --
none of it comes from the oracle or from running the program. That is what
makes this layer independent of test_fuzz.py, which does use the oracle.

usage: python3 test_units.py [path/to/bsq] [-v]
"""
import os
import subprocess
import sys
import tempfile

BSQ = sys.argv[1] if len(sys.argv) > 1 else "../solution/bsq"
VERBOSE = "-v" in sys.argv[2:]

# The subject's one genuine ambiguity, see README.md:
#
#   "When your program receives more than one map in argument, each solution
#    or 'map error' must be followed by a line break."
#
# "none"  -- every line already ends in \n, so maps follow each other directly.
# "blank" -- an extra \n on stdout between maps, as a separator.
#
# Both readings exist in solutions to this exact subject, so the suite can
# validate either: python3 test_units.py <bsq> --sep blank
SEP = "\n" if "--sep" in sys.argv and "blank" in sys.argv else ""

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


def run(args, stdin=""):
    p = subprocess.run([BSQ] + args, input=stdin, capture_output=True, text=True)
    return p.stdout, p.stderr, p.returncode


def check(label, args, stdin, out, err, rc=0):
    got_out, got_err, got_rc = run(args, stdin)
    ok = (got_out, got_err, got_rc) == (out, err, rc)
    detail = ""
    if not ok:
        detail = ("        stdin:  %r\n"
                  "        expected: out=%r err=%r rc=%d\n"
                  "        got:      out=%r err=%r rc=%d"
                  % (stdin, out, err, rc, got_out, got_err, got_rc))
    report(ok, label, detail)


# A map given on stdin (no arguments) and the same map given as a file must
# behave identically, so every fixed case below is run both ways.
def both(label, data, out, err, rc=0):
    check(label + "  [stdin]", [], data, out, err, rc)
    with tempfile.NamedTemporaryFile("w", suffix=".map", delete=False) as fh:
        fh.write(data)
        path = fh.name
    try:
        check(label + "  [file]", [path], "", out, err, rc)
    finally:
        os.unlink(path)


MAPERR = "map error\n"


def cat(*chunks):
    """Join the stdout each map produces, in argument order. A map that errored
    contributes an empty chunk -- its message went to stderr -- but it still
    takes part in the separation, which is what makes SEP observable."""
    return SEP.join(chunks)

# ---------------------------------------------------------------------------

def testValidMaps():
    section("Valid maps")

    both("1x1 empty cell becomes the square",
         "1 . o x\n.\n", "x\n", "")

    both("1x1 obstacle: no square, map unchanged",
         "1 . o x\no\n", "o\n", "")

    both("2x2 all empty is one 2x2 square",
         "2 . o x\n..\n..\n", "xx\nxx\n", "")

    # An obstacle in a corner caps the square at 1x1; the winner is the
    # topmost then leftmost empty cell, which is (0,1) and not (1,1).
    both("2x2 with a corner obstacle picks the top-left-most 1x1",
         "2 . o x\no.\n..\n", "ox\n..\n", "")

    both("3x3 all empty",
         "3 . o x\n...\n...\n...\n", "xxx\nxxx\nxxx\n", "")

    # A centre obstacle blocks every 2x2, so the answer is 1x1 at (0,0).
    both("3x3 with a centre obstacle",
         "3 . o x\n...\n.o.\n...\n", "x..\n.o.\n...\n", "")

    # Two 2x2 squares fit side by side; the leftmost wins.
    both("tie on rows is broken by the leftmost column",
         "2 . o x\n...\n...\n", "xx.\nxx.\n", "")

    # Two 2x2 squares stack vertically; the topmost wins.
    both("tie on columns is broken by the topmost row",
         "3 . o x\n..\n..\n..\n", "xx\nxx\n..\n", "")

    both("a map with no empty cell is printed unchanged",
         "2 . o x\noo\noo\n", "oo\noo\n", "")

    both("a single wide row",
         "1 . o x\n.....\n", "x....\n", "")

    both("a single tall column",
         "3 . o x\n.\n.\n.\n", "x\n.\n.\n", "")

    section("Unusual but legal characters")

    # "The characters can be any printable character, even numbers."
    both("digits as the three map characters",
         "2 1 2 3\n11\n21\n", "31\n21\n", "")

    # The first line is read by position, so a map character may be a space.
    both("space as the empty character",
         "2   o x\n  \n o\n", "x \n o\n", "")

    both("space as the obstacle character",
         "2 .   x\n..\n. \n", "x.\n. \n", "")

    both("leading zeros in the line count",
         "02 . o x\n..\n..\n", "xx\nxx\n", "")

    both("punctuation as map characters",
         "2 * # @\n**\n*#\n", "@*\n*#\n", "")


def testInvalidMaps():
    section("Invalid maps (map error on stderr)")

    both("empty file", "", "", MAPERR)
    both("header only, no map", "3 . o x\n", "", MAPERR)
    both("no newline after the header", "2 . o x", "", MAPERR)
    both("no header at all", "..\n..\n", "", MAPERR)

    section("Invalid first line")

    both("zero lines announced", "0 . o x\n", "", MAPERR)
    both("negative line count", "-2 . o x\n..\n..\n", "", MAPERR)
    both("non-numeric line count", "a . o x\n.\n", "", MAPERR)
    both("no line count at all", " . o x\n.\n", "", MAPERR)
    both("a character is missing", "2 . o\n..\n..\n", "", MAPERR)
    both("a fourth character is present", "2 . o x y\n..\n..\n", "", MAPERR)
    both("trailing garbage on the first line", "2 . o xy\n..\n..\n", "", MAPERR)
    both("tabs instead of spaces", "2\t.\to\tx\n..\n..\n", "", MAPERR)
    both("no separator after the line count", "2-. o x\n..\n..\n", "", MAPERR)
    both("no separator after the empty character", "2 .-o x\n..\n..\n", "", MAPERR)
    both("no separator after the obstacle character", "2 . o-x\n..\n..\n", "", MAPERR)
    both("extra characters after the full character", "2 . o xyz\n..\n..\n", "", MAPERR)
    both("two spaces between fields", "2  . o x\n..\n..\n", "", MAPERR)
    both("a line count that cannot be a count", "9999999999 . o x\n..\n..\n", "", MAPERR)

    section("Two identical characters")

    both("empty == obstacle", "2 . . x\n..\n..\n", "", MAPERR)
    both("empty == full", "2 . o .\n..\n..\n", "", MAPERR)
    both("obstacle == full", "2 . o o\n..\n..\n", "", MAPERR)
    both("all three identical", "2 . . .\n..\n..\n", "", MAPERR)

    section("Invalid body")

    both("lines of different lengths", "2 . o x\n..\n.\n", "", MAPERR)
    both("a longer line after a shorter one", "2 . o x\n.\n..\n", "", MAPERR)
    both("fewer lines than announced", "3 . o x\n..\n..\n", "", MAPERR)
    both("more lines than announced", "2 . o x\n..\n..\n..\n", "", MAPERR)
    both("an empty line in the map", "2 . o x\n\n\n", "", MAPERR)
    both("a zero-width map", "1 . o x\n\n", "", MAPERR)
    both("the full character appears in the map", "2 . o x\n.x\n..\n", "", MAPERR)
    both("an unknown character in the map", "2 . o x\n.z\n..\n", "", MAPERR)
    both("a carriage return in the map", "2 . o x\n.\r\n..\n", "", MAPERR)
    both("no line break at the end of the last line", "2 . o x\n..\n..", "", MAPERR)
    both("a character that is neither empty nor obstacle", "2 . o x\n \n \n", "", MAPERR)


def testArguments():
    section("Arguments and streams")

    files = []

    def tmp(data):
        fh = tempfile.NamedTemporaryFile("w", suffix=".map", delete=False)
        fh.write(data)
        fh.close()
        files.append(fh.name)
        return fh.name

    good1 = tmp("2 . o x\n..\n..\n")
    good2 = tmp("1 . o x\n...\n")
    bad = tmp("2 . o x\n..\n.\n")

    try:
        check("one file", [good1], "", "xx\nxx\n", "")

        # The separation convention, whichever one SEP selects.
        check("two valid files",
              [good1, good2], "", cat("xx\nxx\n", "x..\n"), "")

        check("valid then invalid",
              [good1, bad], "", cat("xx\nxx\n", ""), "map error\n")

        check("invalid then valid",
              [bad, good1], "", cat("", "xx\nxx\n"), "map error\n")

        check("invalid, valid, invalid",
              [bad, good1, bad], "", cat("", "xx\nxx\n", ""), "map error\nmap error\n")

        check("the same file twice",
              [good1, good1], "", cat("xx\nxx\n", "xx\nxx\n"), "")

        check("a file that does not exist",
              ["./no_such_map_zzz"], "", "", "map error\n")

        check("a directory instead of a file",
              ["."], "", "", "map error\n")

        check("a missing file among valid ones",
              [good1, "./no_such_map_zzz", good2], "",
              cat("xx\nxx\n", "", "x..\n"), "map error\n")

        # With no arguments the map comes from stdin.
        check("no arguments reads stdin", [], "2 . o x\n..\n..\n", "xx\nxx\n", "")
        check("no arguments, empty stdin", [], "", "", "map error\n")

        # A map arriving through a pipe rather than a seekable file.
        p = subprocess.run("printf '%s' '2 . o x\n..\n..\n' | " + BSQ,
                           shell=True, capture_output=True, text=True)
        report((p.stdout, p.stderr) == ("xx\nxx\n", ""),
               "a map piped into stdin",
               "        got: out=%r err=%r" % (p.stdout, p.stderr))
    finally:
        for f in files:
            os.unlink(f)


def testSubjectExample():
    section("The subject's own example")

    here = os.path.dirname(os.path.abspath(__file__))
    with open(os.path.join(here, "maps", "subject.map")) as fh:
        data = fh.read()
    with open(os.path.join(here, "expected_subject.txt")) as fh:
        want = fh.read()

    both("the 9x27 example from subject.txt", data, want, "")

    # The winning square is 7x7 at rows 0..6, columns 5..11: nothing else and
    # nothing more was changed.
    got, _, _ = run([os.path.join(here, "maps", "subject.map")])
    lines = got.split("\n")[:-1]
    report(len(lines) == 9, "the answer still has 9 lines")
    report(all(len(l) == 27 for l in lines), "every line is still 27 wide")
    report(sum(l.count("x") for l in lines) == 49, "exactly 49 cells were filled")
    report(all(l[5:12] == "xxxxxxx" for l in lines[:7]),
           "the square sits at rows 0-6, columns 5-11")
    report(lines[7].count("x") == 0 and lines[8].count("x") == 0,
           "the last two rows are untouched")


def testLargeMaps():
    section("Larger maps")

    # A 200x200 board of empties: the answer is the whole board.
    n = 200
    data = "%d . o x\n" % n + "".join("." * n + "\n" for _ in range(n))
    want = "".join("x" * n + "\n" for _ in range(n))
    check("200x200 all empty  [stdin]", [], data, want, "")

    # One obstacle on the main diagonal splits the board into two triangles;
    # the biggest square is then bounded by it.
    n = 60
    rows = []
    for i in range(n):
        rows.append("".join("o" if i == n // 2 and j == n // 2 else "." for j in range(n)))
    data = "%d . o x\n" % n + "".join(r + "\n" for r in rows)
    got, err, rc = run([], data)
    report(err == "" and rc == 0, "60x60 with one obstacle runs cleanly")
    filled = sum(r.count("x") for r in got.split("\n"))
    # The obstacle is at (30, 30). The biggest empty square is 30 wide: rows
    # 0..29 x columns 0..29 is the topmost-leftmost one.
    report(filled == 30 * 30, "60x60 with a central obstacle gives a 30x30 square",
           "        filled %d cells" % filled)
    lines = got.split("\n")[:-1]
    report(all(lines[i][:30] == "x" * 30 for i in range(30)),
           "and it is the top-left 30x30 block")

    # A long thin map: 1 row, 5000 columns.
    data = "1 . o x\n" + "." * 5000 + "\n"
    want = "x" + "." * 4999 + "\n"
    check("1x5000 row", [], data, want, "")

    # A tall thin map: 5000 rows, 1 column.
    data = "5000 . o x\n" + ".\n" * 5000
    want = "x\n" + ".\n" * 4999
    check("5000x1 column", [], data, want, "")


def main():
    if not os.path.exists(BSQ):
        print("%s[KO]%s no such binary: %s" % (RED, OFF, BSQ))
        return 1

    testValidMaps()
    testInvalidMaps()
    testArguments()
    testSubjectExample()
    testLargeMaps()

    total = passed + failed
    print("\n-----------------------------")
    if failed == 0:
        print("%s%d/%d checks passed%s" % (GREEN, passed, total, OFF))
        return 0
    print("%s%d/%d checks passed (%d failed)%s" % (RED, passed, total, failed, OFF))
    return 1


if __name__ == "__main__":
    sys.exit(main())
