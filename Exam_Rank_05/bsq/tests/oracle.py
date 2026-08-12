#!/usr/bin/env python3
"""Reference bsq, written from ../subject.txt rather than from ../solution/bsq.c.

Two independent solvers live here:

  * `biggest_brute` tries every top-left corner and grows a square out of it,
    checking every cell it covers -- O(rows * cols * side^2) and obviously
    correct by reading it.
  * `biggest_dp` is the O(rows * cols) recurrence.

The fuzzer runs both on every small map and requires them to agree, so the
oracle is itself under test rather than being trusted.

Exports:
    solve(data) -> str | None   the solved map, or None if the map is invalid
"""


def getlines(data):
    """Split the way getline() does: each chunk keeps its '\\n', and a final
    chunk with no newline is still returned (so it can be rejected)."""
    out, start = [], 0
    for i, ch in enumerate(data):
        if ch == "\n":
            out.append(data[start:i + 1])
            start = i + 1
    if start < len(data):
        out.append(data[start:])
    return out


def parse(data):
    """Validate a map file. Returns (grid, empty, obstacle, full) or None.

    The rules, straight from the subject:
      - the first line is "<lines> <empty> <obstacle> <full>", space separated;
      - there is at least one line of at least one box;
      - every line has the same length and ends with a line break;
      - the only characters in the map are the empty and obstacle characters;
      - the map is invalid if a character is missing from the first line, or if
        two of the three characters are identical.
    """
    lines = getlines(data)
    if not lines:
        return None

    head = lines[0]
    if not head.endswith("\n"):
        return None
    n = 0
    while n < len(head) and "0" <= head[n] <= "9":
        n += 1
    if n == 0:
        return None
    # "<digits>", then exactly " c" three times, then the newline: nothing else
    # fits on the first line, which is also what lets a map character be a space.
    if len(head) != n + 7:
        return None
    if head[n] != " " or head[n + 2] != " " or head[n + 4] != " ":
        return None
    rows = int(head[:n])
    if rows < 1:
        return None
    empty, obstacle, full = head[n + 1], head[n + 3], head[n + 5]
    if len({empty, obstacle, full}) != 3:
        return None

    body = lines[1:]
    if len(body) != rows:
        return None

    grid, cols = [], None
    for line in body:
        if not line.endswith("\n"):
            return None
        row = line[:-1]
        if cols is None:
            cols = len(row)
        if len(row) != cols or cols < 1:
            return None
        for ch in row:
            if ch != empty and ch != obstacle:
                return None
        grid.append(row)

    return grid, empty, obstacle, full


def biggest_brute(grid, empty):
    """Every top-left corner, scanned row-major; a strictly larger square wins,
    so the first square of maximal size -- topmost, then leftmost -- is kept."""
    rows, cols = len(grid), len(grid[0])
    best = top = left = 0
    for i in range(rows):
        for j in range(cols):
            side = 0
            while i + side < rows and j + side < cols:
                # grow by one: check the new bottom row and the new right column
                ok = all(grid[i + k][j + side] == empty for k in range(side + 1)) and \
                     all(grid[i + side][j + k] == empty for k in range(side + 1))
                if not ok:
                    break
                side += 1
            if side > best:
                best, top, left = side, i, j
    return best, top, left


def biggest_dp(grid, empty):
    """dp[i][j] = side of the biggest square whose bottom-right corner is (i, j).

    Scanned row-major with a strict '>', which keeps the first maximal
    bottom-right corner; for a fixed side that is also the first maximal
    top-left corner.
    """
    rows, cols = len(grid), len(grid[0])
    prev = [0] * cols
    best = bottom = right = 0
    for i in range(rows):
        cur = [0] * cols
        for j in range(cols):
            if grid[i][j] == empty:
                if i and j:
                    cur[j] = 1 + min(prev[j], cur[j - 1], prev[j - 1])
                else:
                    cur[j] = 1
                if cur[j] > best:
                    best, bottom, right = cur[j], i, j
        prev = cur
    if best == 0:                       # no empty cell at all: nothing to fill
        return 0, 0, 0
    return best, bottom - best + 1, right - best + 1


def render(grid, top, left, side, full):
    out = [list(r) for r in grid]
    for i in range(top, top + side):
        for j in range(left, left + side):
            out[i][j] = full
    return "".join("".join(r) + "\n" for r in out)


def solve(data, cross_check=False):
    """The solved map for a valid map file, else None."""
    parsed = parse(data)
    if parsed is None:
        return None
    grid, empty, _obstacle, full = parsed

    side, top, left = biggest_dp(grid, empty)
    if cross_check:
        b = biggest_brute(grid, empty)
        if b != (side, top, left):
            raise AssertionError(
                "the two reference solvers disagree: brute=%r dp=%r on %r"
                % (b, (side, top, left), grid))
    return render(grid, top, left, side, full)


if __name__ == "__main__":
    import sys
    data = sys.stdin.read()
    out = solve(data, cross_check=True)
    if out is None:
        sys.stderr.write("map error\n")
    else:
        sys.stdout.write(out)
