# mini_serv — Design Documentation

A single-threaded TCP chat relay for 42 Exam Rank 06: clients connect on
`127.0.0.1:<port>`, every message a client sends is re-broadcast to every other
client with a `client %d: ` prefix on each line. One file, norminette-clean,
zero global variables, zero allocations after startup.

```
            ┌──────────────────────────── main loop ───────────────────────────┐
            │  rd = act ; wr = act        (readiness snapshots)                │
            │  select(1024, &rd, &wr)     (sleep until something is ready)     │
            │  first ready fd:                                                 │
            │     fd == sock ──► client_event()  accept + "just arrived"       │
            │     else       ──► handle_msg()    recv + line relay / "left"    │
            │  break ──► fresh snapshots                                       │
            └──────────────────────────────────────────────────────────────────┘
```

---

## 1. The event model

### Why `select()`
The subject allows exactly one readiness primitive: `select`. No `poll`, no
`epoll`, no threads, no `fork`. The whole server is therefore **one process,
one thread, one blocking point**: the `select()` call.

### Readiness snapshots
`select()` *mutates* the fd-sets you pass it. The server keeps three sets in
one array (`fd_set sets[3]`):

| index | role | lifetime |
|-------|------|----------|
| `sets[0]` (`act`) | master set — listening socket + every connected client | permanent, mutated only on accept/disconnect |
| `sets[1]` (`rd`)  | per-round copy handed to `select` for readability | one loop iteration |
| `sets[2]` (`wr`)  | per-round copy handed to `select` for writability | one loop iteration |

Every iteration re-copies `act` into `rd`/`wr`, so the master set is never
corrupted by the kernel's answer.

### One event per `select()` round (`break`)
After handling **one** ready fd the inner scan `break`s and the loop
re-selects. This is a deliberate correctness choice: the moment a client is
accepted or closed, the current `rd`/`wr` snapshots are stale. Re-selecting
after every mutation means **no code path ever consults a stale set** — the
classic source of "send to a just-closed fd" bugs in naive sweeps. The cost is
a few extra `select()` syscalls under load; since pending fds make `select`
return immediately, the overhead is microseconds on localhost.

### The non-blocking rule
The subject says fds behave non-blocking *as long as `select` was called
first*, and forbids checking `EAGAIN`. The server obeys by construction:

- `recv` is only ever called on an fd that was in the **read**-ready snapshot;
- `send` is only ever attempted on fds in the **write**-ready snapshot;
- `accept` is only called when the listening socket is read-ready.

No `fcntl`, no `O_NONBLOCK`, no `errno` inspection anywhere.

---

## 2. State layout — no globals

Norminette 3.3.59 forbids `typedef` **and** `struct` declarations in `.c`
files, and a header would need `#define` (include guard), which the subject
bans. So state is not a struct and not global — it lives in `main` and travels
by parameter (the Norm allows 4 per function):

```
main() stack                                heap (one malloc at startup)
┌──────────────────────────────┐            ┌────────────────────────────────┐
│ fd_set sets[3]   act|rd|wr   │            │ 1024 × 100 000 B               │
│ int    id[1024]  fd → id     │            │ per-client line buffers        │
│ int    sock, fd              │   msg ───► │ msg[fd] = unfinished line of   │
│ char (*msg)[100000]          │            │           client on that fd    │
└──────────────────────────────┘            └────────────────────────────────┘
```

Design details:

- **`id[]` and `msg[]` are indexed by fd**, not by client number. An fd is a
  small dense integer handed out by the kernel — it *is* a perfect hash. No
  lookup structure, no iteration to find a client: every access is `O(1)`.
- **`char (*msg)[100000]`** — pointer to rows of 100 000 bytes. 102.4 MB does
  not fit on an 8 MB stack, so it is the program's **only allocation**,
  requested once in `create_server`. `malloc` failure hits the subject's
  "Fatal error" path. It is plain `malloc`, not `calloc`: rows are initialized
  (`msg[cl][0] = 0`) the moment a client is accepted, so untouched rows never
  cost physical memory (lazy pages → tiny RSS despite 102 MB virtual).
- **The id counter is `static int nid;` inside `client_event`** — function
  scope, zero-initialized by C, invisible to the rest of the file. This is
  exactly the mechanism `get_next_line` is built on, so it is norm-legal and
  idiomatic 42.
- **`client_event` re-derives the listening socket** instead of receiving it
  as a 5th parameter: the listening fd is always the *lowest* bit in `act`.
  Proof: it is created before any client, fds 0–2 are stdio, the kernel always
  hands out the lowest free fd, and the listening fd is never closed — so no
  client fd can ever be smaller. The scan is a 3-line `while` over a bitmask.

---

## 3. The algorithms

### 3.1 Arrival — `client_event` (fd == sock)

```
cl = accept(sock)
id[cl]  = nid++          client numbers are monotonic: 0, 1, 2, … (never reused)
msg[cl][0] = '\0'        reset the line buffer (also wipes leftovers if the
                         kernel recycled the fd of a previous client)
FD_SET(cl, act)
broadcast "server: client %d just arrived\n"   (to everyone else)
```

The newcomer never receives its own arrival notice: it is excluded as the
`except` argument **and** it is absent from the `wr` snapshot, which was taken
before the accept.

### 3.2 Departure (recv ≤ 0)

`recv` returning `0` (orderly shutdown) or `-1` (error) both mean the client
is gone — one code path:

```
broadcast "server: client %d just left\n"
FD_CLR(fd, act)
close(fd)
```

No buffer cleanup needed at departure: the *next* client accepted on this fd
resets `msg[fd][0]` itself.

### 3.3 Line assembly — the core loop of `handle_msg`

**Invariant: `msg[fd]` is always a NUL-terminated string containing the
client's current unfinished line.** Established at accept, preserved by every
`handle_msg` call.

```c
j = strlen(msg[fd]);            /* resume where the partial line ended   */
while (++i < ret)               /* walk the recv'd chunk byte by byte    */
{
    msg[fd][j] = rb[i];
    if (rb[i] == '\n')          /* complete line!                        */
    {
        msg[fd][j] = 0;         /* strip the \n, terminate               */
        sprintf(sb, "client %d: %s\n", id[fd], msg[fd]);
        send_all(sets, fd, sb); /* flush immediately                     */
        j = -1;                 /* restart the line at index 0           */
    }
    j++;
}
msg[fd][j] = 0;                 /* re-establish the invariant            */
```

Worked example — client 0 sends `"par"`, then `"tial\n"` in a second packet:

| step | recv chunk | msg[0] after | broadcast |
|------|-----------|--------------|-----------|
| 1 | `"par"` | `"par"` | — (no newline yet) |
| 2 | `"tial\n"` | `""` | `client 0: partial\n` |

And `"hello\nworld\n"` in **one** packet produces two broadcasts in the same
pass — the scan handles any number of `\n` per chunk.

The final `msg[fd][j] = 0` does double duty: it terminates a trailing partial
line **and** resets the buffer to `""` when the chunk ended exactly on a
newline (`j` is back at 0), which is what keeps stale data from resurfacing.

### 3.4 Broadcast — `send_all`

```c
while (++fd < 1024)
    if (FD_ISSET(fd, &sets[2]) && fd != except)
        send(fd, sb, strlen(sb), MSG_NOSIGNAL);
```

- Gating on the **write snapshot** honours the subject's lazy-client rule: a
  client that stopped reading has a full kernel pipe, is not write-ready, and
  is silently *skipped* — never blocked on, never disconnected.
- `MSG_NOSIGNAL` closes the classic crash hole: `send` to a peer that vanished
  between `select` and `send` yields `-1` instead of a process-killing
  `SIGPIPE`. The `-1` is deliberately ignored — the dead client will show up
  read-ready and be reaped by the `recv ≤ 0` path within one loop turn.
- Errors after the server starts accepting are *not* fatal by design; the
  subject reserves "Fatal error" for startup failures and allocation failures.

---

## 4. Why this is faster than the usual solutions

### 4.1 vs. the canonical `str_join` / `extract_message` solution

The solution most students build from the provided `main.c` helpers stores
each client's pending data in a heap string and, on **every** `recv`:

1. `malloc`s a new buffer of `old + new` size (`str_join`),
2. copies the **entire existing buffer** plus the new chunk into it,
3. frees the old one,
4. then `extract_message` re-scans from index 0 for each `\n` and
   `calloc`+`strcpy`s the remainder into yet another fresh buffer per line.

For a line of length `L` arriving in `k` fragments that is
`O(k·L)` bytes copied — **quadratic** in the number of fragments — plus 2+
allocator round-trips per recv, plus repeated rescans of the same bytes.

This server keeps a fixed per-client row and appends in place:

| cost per… | this server | str_join/extract_message |
|---|---|---|
| byte received | **1 write** into `msg[fd]` (each byte touched once) | `O(k)` copies (whole-buffer copy per fragment) |
| `recv` call | **0 allocations** | 1–2 `malloc`/`calloc` + full copy + `free` |
| complete line | 1 `sprintf` + 1 `send` per ready peer | same, **plus** remainder copy + rescan from 0 |
| memory | fixed, allocated once | fragmenting heap churn under load |

### 4.2 Immediate flush

The subject warns: *"our tester is expecting that you send the messages as
fast as you can. Don't do un-necessary buffer."* A completed line is
broadcast **inside the scan loop**, the instant its `\n` is seen — not after
the chunk is fully parsed, not queued, not coalesced. Buffering exists only
where it is *semantically required*: the unfinished tail of a line (you cannot
prefix a line you haven't fully seen without corrupting interleaving between
clients).

### 4.3 No hidden syscall or signal machinery

- One `sprintf` per line, one `send` per ready recipient — no intermediate
  formatting buffers.
- No signal handlers, no `errno` churn, no `EAGAIN` retry loops (banned
  anyway) — `MSG_NOSIGNAL` is a flag, not a code path.
- `select(1024, …)` with a constant `nfds` trades a 128-byte kernel bitmask
  scan for not maintaining `max_fd` bookkeeping — measurably free, and it
  deleted a variable and two branches.

### 4.4 What was *not* done in the name of speed

An unbuffered relay (forward partial chunks immediately, track
"mid-line" per client) would be marginally faster still, but **corrupts
output** when two clients' partial lines interleave on a receiver's stream —
receivers would see `client 0: helclient 1: hi` fragments. Correctness wins;
per-line flushing is the fastest *correct* strategy.

---

## 5. Trade-offs (deliberate ceilings)

| trade-off | why it's acceptable | upgrade path if ever needed |
|---|---|---|
| A single line longer than 100 000 chars overflows its row | exam tester sends normal lines; identical ceiling exists in the classic passing solutions | per-client `realloc`-doubling buffer |
| fds ≥ 1024 unsupported | hard limit of `select`/`FD_SETSIZE` itself — every `select` solution shares it | `poll`/`epoll` (not allowed by subject) |
| Lazy client misses messages sent while its pipe is full | subject forbids disconnecting or blocking on it; queuing would need unbounded per-client RAM | bounded ring buffer per client |
| One event per `select` round → extra syscalls | buys never-stale fd-sets; `select` with pending events returns immediately | full-sweep with careful stale-set hygiene |
| 102 MB virtual reservation | one `malloc`, lazy pages → RSS stays at a few MB until buffers are actually written | smaller rows + `realloc` growth |
| `msg` block never freed | the server has no exit path; valgrind: 0 bytes definitely/indirectly/possibly lost (block is *still reachable* at kill) | free on a signal-driven shutdown path |
| 1024-iteration scans instead of `max_fd` tracking | constant ~µs cost; removed state and branches | reintroduce `max_fd` |
| Functions shaped by the Norm (5 per file, ≤25 lines, ≤4 params, no `for`) | required for norminette `OK!`; dictated the parameter threading and the dual-role `client_event` | none — that's the constraint |

---

## 6. Subject-compliance map

| subject requirement | where satisfied |
|---|---|
| `Wrong number of arguments\n` on stderr, exit 1, when argc ≠ 2 | top of `create_server` |
| `Fatal error\n` on stderr, exit 1, on syscall/alloc failure before accepting | combined check in `create_server` (`socket`, `malloc`, `bind`, `listen`) |
| Listen only on `127.0.0.1` | `htonl(2130706433)` = 0x7F000001 |
| ids start at 0, increment forever, never reused | `static int nid` in `client_event` |
| `server: client %d just arrived/left\n` to all others | `client_event`, both branches |
| `client %d: ` before **every** line, multi-`\n` messages | byte-scan in `handle_msg` |
| never block, never check EAGAIN, don't disconnect lazy clients | select-gated `recv`/`send`, write-snapshot filter |
| no `#define`, only allowed functions | whole file (`MSG_NOSIGNAL` is a flag to the allowed `send`) |
| no memory / fd leaks | one reachable block, every client fd closed on departure |

---

## 7. Verification

`test.sh` (scratchpad) exercises the full protocol against the real binary:

- no args → `Wrong number of arguments`, exit 1
- unbindable port → `Fatal error`, exit 1
- arrival broadcast reaches the earlier client only
- `"hello\nworld\n"` in one packet → two prefixed lines
- `"par"` + `"tial\n"` split across packets → one correct `client 0: partial`
- disconnect broadcast
- valgrind: **0 allocs at runtime beyond the startup block, 0 bytes lost, no
  fd leaks** (only stdio + the listening socket open at kill)

Companion diagram: **`mini_serv.excalidraw`** (open at excalidraw.com or with
the VS Code Excalidraw extension) — the event loop, both event paths, the
line-assembly state machine, memory layout, and the speed/trade-off summary.
