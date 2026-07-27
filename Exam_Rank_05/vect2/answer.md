# Why `c++ *.cpp` on vect2.cpp produces ~150 lines of errors

Short version: **there are only 7 real bugs**. C++ compilers are bad at
recovering gracefully from a syntax error — once the parser gets confused on
one line, it keeps guessing and spits out follow-up errors for code that
would otherwise be fine. And for the `operator<<` bugs specifically, GCC
prints a "candidate" note for *every overload of `operator<<` that exists in
the standard library* (there are 33 of them) each time it fails to find a
match — that's not 33 bugs, it's 1 bug reported very verbosely, 4 times.

Read this top to bottom, in the order the bugs appear in the file.

---

## Bug 1 — line 9: missing `<<`

```cpp
std::cout << "x: " x << "y: " y << std::endl;
```

You forgot the stream operator between the string and the variable. It should be:

```cpp
std::cout << "x: " << x << " y: " << y << std::endl;
```

**Why this matters more than it looks:** a missing `<<` isn't just "one
error." The parser sees `"x: " x` and has no idea what that means
syntactically, so it reports `expected ';' before 'x'` and gives up
recovering the rest of the statement. This is bug #1 of the cascade.

---

## Bug 2 — line 15-16: missing semicolon

```cpp
vect2::vect2 (const vect2& other) : vect2(other.x, other.y) {

    std::cout << "copy constructor called." << std::endl
}
```

No `;` after `std::endl`. Every statement in C++ needs one. Should be:

```cpp
std::cout << "copy constructor called." << std::endl;
```

---

## Bug 3 — line 24: malformed operator= definition

```cpp
vect2::vect2& operator=(const vect2& other) {
```

This is the one that produced `'vect2::vect2' names the constructor, not the
type`. The scope-resolution syntax for defining a member function outside
the class is:

```
ReturnType ClassName::functionName(...)
```

You wrote `vect2::vect2& operator=(...)` — the compiler reads `vect2::vect2`
as "the constructor of vect2" (constructor lookup), not as "a `vect2&`
qualified by the `vect2::` scope". What you meant was:

```cpp
vect2& vect2::operator=(const vect2& other) {
```

**There's a second problem hiding here too:** your header currently declares

```cpp
vect2 operator=(const vect2& other);   // returns by value
```

but your `.cpp` is trying to return `vect2&` (a reference). Once you fix the
syntax, these two need to *match* — the declaration in the `.hpp` and the
definition in the `.cpp` must have the identical signature. Given the
comment you already wrote in the file explaining why returning by value is
non-canonical, you clearly intend to return by reference — so update the
header too:

```cpp
// vect2.hpp
vect2& operator=(const vect2& other);
```

---

## Bug 4 — lines 36-42: a leftover/duplicate `main` using a typo'd type

```cpp
int main(void) {
    Vect2 v1;          // capital V — never declared, your class is `vect2`
    Vect2 v2(1, 2);
    const Vect2 v3(v2);
    vect2 v4 = v2;      // v2 doesn't exist under this name -> "was not declared"
}
```

`Vect2` (capital V) was never declared anywhere — your class is `vect2`
(lowercase). This whole block looks like scratch/leftover code you didn't
mean to keep, especially since a **second, correct** `main` immediately
follows it at line 44. Delete this entire first `main` (lines 36-42).

This single typo (`Vect2` instead of `vect2`) is what triggers:
- `'Vect2' was not declared in this scope`
- `expected ';' before 'v2'` (parser recovery noise)
- `'Vect2' does not name a type`
- `'v2' was not declared` (because the `Vect2 v2(1,2)` line failed to parse, so `v2` never got declared)

All from one typo, all in dead code that shouldn't exist.

---

## Bug 5 — line 44: redefinition of `main`

```cpp
int main(void) {   // line 36 — the bogus one
    ...
}

int main(void) {   // line 44 — the real one
    ...
}
```

You can only have one `main`. This error goes away automatically once you
delete the bogus `main` from Bug 4 — it's not a separate fix.

---

## Bug 6 — line 50: `std:endl` (single colon)

```cpp
std::cout << "v1: " << v1 << std:endl;
                                ^^^^
```

Single colon `std:endl` instead of double colon `std::endl`. A single `:`
is not the scope-resolution operator — the compiler correctly complains
`found ':' in nested-name-specifier, expected '::'`. Fix:

```cpp
std::cout << "v1: " << v1 << std::endl;
```

---

## Bug 7 (the big one) — no `operator<<` exists for `vect2`

This is the one generating the massive "there are 33 candidates" blocks,
four times over (once each for `v1`, `v2`, `v3`, `v4`).

```cpp
std::cout << "v1: " << v1 << std::endl;
```

For `std::cout << v1` to compile, there must be a function somewhere with
the shape:

```cpp
std::ostream& operator<<(std::ostream& os, const vect2& v);
```

**You never wrote one**, and your class doesn't declare it as a `friend`
either. So when the compiler tries to resolve `std::cout << v1`, it looks at
every `operator<<` it knows about — and the C++ standard library defines
*a lot* of them (for `int`, `double`, `bool`, `const char*`, `std::string`,
manipulators like `std::endl`, etc. — 33 in total on your standard library
version). None of them accept a `vect2`, so GCC dumps all 33 as failed
candidates, with a "note" explaining why each one doesn't match. That's
where the bulk of your error output volume comes from — it's diagnostic
detail for **one missing function**, repeated for each of the 4 print
statements.

### The fix

Add a declaration in `vect2.hpp` (as a `friend` so it can reach the private
`x`/`y` members):

```cpp
class vect2 {
    private:
        int x;
        int y;

    public:
        vect2();
        vect2(int x, int y);
        vect2(const vect2& other);
        vect2& operator=(const vect2& other);
        ~vect2();

        friend std::ostream& operator<<(std::ostream& os, const vect2& v);
};
```

And define it in `vect2.cpp`:

```cpp
std::ostream& operator<<(std::ostream& os, const vect2& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}
```

This is a free function (not a member), because the left-hand side of `<<`
is `std::ostream`, not `vect2` — you can't write it as `vect2::operator<<`.

---

## Summary table

| # | Line(s) | Bug | Fix |
|---|---------|-----|-----|
| 1 | 9 | missing `<<` before `x` and `y` | add the operators |
| 2 | 15-16 | missing `;` after `std::endl` | add `;` |
| 3 | 24 | `vect2::vect2&` instead of `vect2&\ vect2::` + return-type mismatch with header | fix syntax, make header/cpp both return `vect2&` |
| 4 | 36-42 | leftover duplicate `main` using undeclared `Vect2` | delete the whole block |
| 5 | 44 | redefinition of `main` | resolved automatically once #4 is deleted |
| 6 | 50 | `std:endl` (single colon) | `std::endl` |
| 7 | 50-54 | no `operator<<` overload for `vect2` (root cause of the 33-candidate spam) | add a free `friend operator<<` |

Fix these 7 things in this order (top to bottom) and recompile after each
one or two — you'll watch the error count collapse fast, because most of
what you're seeing right now is downstream noise from a small number of
root causes, not 30+ independent problems.
