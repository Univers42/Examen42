#!/usr/bin/env python3
"""Generate random cases, run the C++ driver, diff every result against python ints."""
import random, subprocess, sys

random.seed(20260725)
cases = []
for _ in range(20000):
    la, lb = random.randint(1, 200), random.randint(1, 200)
    a = str(random.randint(0, 10**la))
    b = str(random.randint(0, 10**lb))
    if random.random() < .15:  # force equal values / equal lengths / zeros
        b = a if random.random() < .5 else str(random.randint(0, 10**la))
    if random.random() < .05:
        a = "0"
    k = str(random.randint(0, 300))
    cases.append((a, b, k))

inp = "".join("%s %s %s\n" % c for c in cases)
out = subprocess.run(["./fuzz"], input=inp, capture_output=True, text=True, check=True).stdout.splitlines()

bad = 0
for (a, b, k), line in zip(cases, out):
    A, B, K = int(a), int(b), int(k)
    got = line.split()
    want = [str(A + B), str(A * 10**K), str(A // 10**K),
            "%d%d%d%d%d%d" % (A < B, A <= B, A > B, A >= B, A == B, A != B)]
    if got != want:
        bad += 1
        print("MISMATCH a=%s b=%s k=%s\n  got  %s\n  want %s" % (a, b, k, got, want))
        if bad > 5:
            break

print("%d cases, %d mismatches" % (len(cases), bad))
sys.exit(1 if bad else 0)
