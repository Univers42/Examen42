#!/usr/bin/env python3
"""bsq tester: fixed edge cases + N random maps diffed against a brute-force
reference.  Usage: python3 test.py [path/to/bsq] [n_random]   (defaults: ./solution/bsq 200)"""
import random, re, string, subprocess, sys

BSQ = sys.argv[1] if len(sys.argv) > 1 else "./solution/bsq"
N = int(sys.argv[2]) if len(sys.argv) > 2 else 200
CHARS = string.digits + string.ascii_letters + string.punctuation


def solve_ref(data):
    """Reference oracle: solved map for a valid map file, else None."""
    m = re.match(r"(\d+) (.) (.) (.)\n", data)
    if not m:
        return None
    rows, e, o, f = int(m.group(1)), m.group(2), m.group(3), m.group(4)
    if rows < 1 or len({e, o, f}) < 3:
        return None
    body = data[m.end():]
    if not body.endswith("\n"):
        return None
    lines = body.split("\n")[:-1]
    cols = len(lines[0]) if lines else 0
    if len(lines) != rows or cols < 1 or any(len(l) != cols for l in lines):
        return None
    if any(c not in (e, o) for l in lines for c in l):
        return None
    grid = [list(l) for l in lines]
    dp = [[0] * cols for _ in range(rows)]
    best = br = bc = 0
    for i in range(rows):
        for j in range(cols):
            if grid[i][j] == e:
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]) if i and j else 1
                if dp[i][j] > best:
                    best, br, bc = dp[i][j], i, j
    for i in range(br - best + 1, br + 1):
        for j in range(bc - best + 1, bc + 1):
            grid[i][j] = f
    return "".join("".join(r) + "\n" for r in grid)


def make_map(rng):
    e, o, f = rng.sample(CHARS, 3)
    rows, cols = rng.randint(1, 15), rng.randint(1, 15)
    body = "".join("".join(rng.choices([e, o], [1 - rng.random() * 0.4, rng.random() * 0.4])[0]
                   for _ in range(cols)) + "\n" for _ in range(rows))
    data = f"{rows} {e} {o} {f}\n{body}"
    if rng.random() < 0.4:  # mutate into a (usually) invalid map; oracle decides
        mut = rng.choice(["no_nl", "extra_line", "resize", "badchar", "dup", "wrong_rows",
                          "blank_first", "split_hdr"])
        if mut == "no_nl":
            data = data[:-1]
        elif mut == "extra_line":
            data += e * cols + "\n"
        elif mut == "resize":
            i = rng.randrange(rows)
            ls = data.split("\n")
            ls[1 + i] = ls[1 + i][:-1] if rng.random() < 0.5 else ls[1 + i] + e
            data = "\n".join(ls)
        elif mut == "badchar":
            data = data[:len(data) - 2] + rng.choice([f, chr(1), " "]) + "\n"
        elif mut == "dup":
            data = data.replace(f" {o} ", f" {e} ", 1)
        elif mut == "wrong_rows":
            data = f"{rows + rng.choice([-1, 1])}" + data[len(str(rows)):]
        elif mut == "blank_first":
            data = "\n" + data
        elif mut == "split_hdr":
            data = data.replace(" ", "\n", 1)
    return data


EDGE = [
    ("empty file", ""),
    ("header only", "3 . o x\n"),
    ("rows 0", "0 . o x\n"),
    ("negative rows", "-2 . o x\n..\n..\n"),
    ("non-numeric rows", "a . o x\n.\n"),
    ("missing char", "2 . o\n..\n..\n"),
    ("duplicate chars", "2 . . x\n..\n..\n"),
    ("header trailing garbage", "2 . o xy\n..\n..\n"),
    ("tab separators", "2\t.\to\tx\n..\n..\n"),
    ("double space in header", "2  . o x\n..\n..\n"),
    ("huge rows", "9999999999 . o x\n..\n..\n"),
    ("unequal lines", "2 . o x\n..\n.\n"),
    ("too few lines", "3 . o x\n..\n..\n"),
    ("empty map line", "2 . o x\n\n\n"),
    ("full char in map", "2 . o x\n.x\n..\n"),
    ("no final newline", "2 . o x\n..\n.."),
    ("1x1 empty", "1 . o x\n.\n"),
    ("1x1 obstacle", "1 . o x\no\n"),
    ("all obstacles", "2 . o x\noo\noo\n"),
    ("digit map chars", "2 1 2 3\n11\n21\n"),
    ("space as empty char", "2   o x\n  \n o\n"),
    ("subject example", "9 . o x\n" + "\n".join(
        ["...........................", "....o......................",
         "............o..............", "...........................",
         "....o......................", "...............o...........",
         "...........................", "......o..............o.....",
         "..o.......o................"]) + "\n"),
]


def check(name, data, fails):
    exp = solve_ref(data)
    p = subprocess.run([BSQ, "/dev/stdin"], input=data, capture_output=True, text=True)
    want_out, want_err = (exp or ""), ("" if exp else "map error\n")
    if (p.stdout, p.stderr) != (want_out, want_err):
        fails.append(name)
        print(f"FAIL {name}\n  map: {data!r}\n  want: {want_out!r} / {want_err!r}\n"
              f"  got:  {p.stdout!r} / {p.stderr!r}")


fails = []
for name, data in EDGE:
    check(name, data, fails)
rng = random.Random()
for k in range(N):
    check(f"random #{k}", make_map(rng), fails)

p = subprocess.run([BSQ, "nonexistent_file_zz"], capture_output=True, text=True)
if (p.stdout, p.stderr) != ("", "map error\n"):
    fails.append("nonexistent file")

print(f"{len(EDGE) + N + 1 - len(fails)}/{len(EDGE) + N + 1} passed" + (" — ALL OK" if not fails else ""))
sys.exit(1 if fails else 0)
