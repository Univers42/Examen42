#!/usr/bin/env python3
"""Reference life, written from ../subject.txt rather than from ../life.c.

Two independent generation steppers live here:

  * `step_count` is the straightforward one: for each cell, count the eight
    neighbours that exist and are alive, then apply the rule.
  * `step_sets` works the other way round -- it walks the live cells and adds
    one to each of their neighbours' tallies, so a cell's count is built up
    rather than read off. Nothing about the loop bounds is shared with the
    first version.

The fuzzer runs both and requires them to agree, so the oracle is itself under
test rather than being trusted.

Exports:
    simulate(w, h, iterations, commands) -> str   the program's expected stdout
"""


def draw(w, h, commands):
    """Run the pen over an empty board and return the live cells.

    From the subject: the pen starts in the top left corner, `w a s d` move it
    up/left/down/right and `x` lifts or lowers it. A cell is drawn when the pen
    is down and standing on it, so lowering the pen draws the cell it is
    already on, and the pen keeps its position while it is outside the board.
    """
    board = [[0] * w for _ in range(h)]
    x = y = 0
    down = False
    for c in commands:
        if c == "x":
            down = not down
        elif c == "w":
            y -= 1
        elif c == "s":
            y += 1
        elif c == "a":
            x -= 1
        elif c == "d":
            x += 1
        # every other character moves nothing and toggles nothing
        if down and 0 <= x < w and 0 <= y < h:
            board[y][x] = 1
    return board


def step_count(board, w, h):
    """One generation, by counting each cell's live neighbours."""
    out = [[0] * w for _ in range(h)]
    for i in range(h):
        for j in range(w):
            n = 0
            for di in (-1, 0, 1):
                for dj in (-1, 0, 1):
                    if di == 0 and dj == 0:
                        continue
                    ni, nj = i + di, j + dj
                    if 0 <= ni < h and 0 <= nj < w:   # outside the board is dead
                        n += board[ni][nj]
            out[i][j] = 1 if (n == 3 or (n == 2 and board[i][j])) else 0
    return out


def step_sets(board, w, h):
    """One generation, by scattering from the live cells into a tally."""
    tally = {}
    for i in range(h):
        for j in range(w):
            if board[i][j]:
                for di in (-1, 0, 1):
                    for dj in (-1, 0, 1):
                        if di or dj:
                            key = (i + di, j + dj)
                            tally[key] = tally.get(key, 0) + 1
    out = [[0] * w for _ in range(h)]
    for i in range(h):
        for j in range(w):
            n = tally.get((i, j), 0)
            out[i][j] = 1 if (n == 3 or (n == 2 and board[i][j])) else 0
    return out


def render(board, w, h):
    """'O' for a live cell, a space for a dead one, a newline after each row."""
    return "".join("".join("O" if board[i][j] else " " for j in range(w)) + "\n"
                   for i in range(h))


def simulate(w, h, iterations, commands, cross_check=False):
    """What ./life w h iterations must print for this command stream.

    A board with no cell prints nothing at all; the same goes for arguments
    that are not positive numbers.
    """
    if w < 1 or h < 1:
        return ""
    board = draw(w, h, commands)
    for _ in range(max(0, iterations)):
        nxt = step_count(board, w, h)
        if cross_check:
            other = step_sets(board, w, h)
            if other != nxt:
                raise AssertionError("the two reference steppers disagree on %r" % board)
        board = nxt
    return render(board, w, h)


if __name__ == "__main__":
    import sys
    w, h, k = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3])
    sys.stdout.write(simulate(w, h, k, sys.stdin.read(), cross_check=True))
