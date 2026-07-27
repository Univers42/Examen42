Good — that's the more useful goal anyway. Here's the mental model, tied to the file you have open.

1. The one idea everything else hangs off
The class is a std::string with a promise attached: the digits are base 10, most-significant first, and there is never a leading zero (zero itself is "0").

That promise is called a class invariant. It's the whole design. Every constructor must establish it, and every operation must preserve it — and in exchange, every operation gets to assume it. That's why printing is just os << b.s with no cleanup: nobody can ever hand you "0042", because no constructor can produce it. When you see a class where one method is suspiciously simple, it's usually because an invariant is doing the work somewhere else.

This is the transferable lesson: pick the representation so that the operations become trivial, rather than picking an obvious representation and writing clever operations.

2. Operator overloading is just functions with weird names
a + b is not magic. The compiler rewrites it into a function call. That's all. Two possible spellings:


a.operator+(b)      // member function — 'a' is the object, 'b' is the argument
operator+(a, b)     // free function   — both are arguments
So an operator is a normal function; the only unusual thing is that you call it with infix syntax. Once you internalise that, questions like "why is << sometimes a shift and sometimes printing?" answer themselves: they're two different overloads, and the compiler picks by argument types. std::cout << b matches operator<<(ostream&, const bigint&); b << 10 matches operator<<(bigint, const bigint&). Same name, different functions.

3. Member or free? There's a rule
If the left operand isn't your type, the operator cannot be a member. A member function's left operand is always *this.

That single rule forces line bigint.hpp:40 to be a free function: in std::cout << b the left operand is a std::ostream, and you can't add members to a class you don't own. So it's free, and marked friend so it can reach the private b.s.

friend means "this function is not a member, but I grant it access to my privates." Nothing more. Declaring it inside the class body (as I did) is just a placement trick — the function is still a free function, it's simply written where it's convenient to read.

For operators that could go either way, the convention is: anything that modifies the left operand is a member; anything symmetric is free. Hence += is a member (line 21) and + is free (line 28).

4. The pattern that generates 13 operators from 3
This is the part actually worth memorising, because it applies to every class you'll ever write.

Write the mutating version by hand. Derive everything else.


bigint& operator+=(const bigint& o);                                  // the real work — 17 lines
friend bigint operator+(bigint a, const bigint& b) { return a += b; } // derived — 1 line
bigint& operator++()      { return *this += 1; }                      // derived — 1 line
bigint operator++(int)    { bigint t(*this); *this += 1; return t; }  // derived — 1 line
Addition logic exists in exactly one place. If it's wrong, it's wrong in one place; fix it once and four operators get fixed.

Note the return types, because they're not arbitrary:

+= returns bigint& — a reference to the object it just modified, so (c += a) can be printed or chained. This is why the subject's main can write std::cout << (c += a).
+ returns bigint by value — it must, because it creates a new number. Returning a reference here would be a dangling reference to a dead local, one of the classic C++ crashes.
Same story for comparison. Write two, derive four:


friend bool operator<(...)  { /* real logic */ }
friend bool operator==(...) { return a.s == b.s; }
friend bool operator>(...)  { return b < a; }        // "a > b" IS "b < a"
friend bool operator<=(...) { return !(b < a); }     // "a <= b" IS "not b < a"
friend bool operator>=(...) { return !(a < b); }
friend bool operator!=(...) { return !(a == b); }
Lines 35–38 contain no logic at all — they're pure algebra on top of line 32. This is why the six comparisons cannot disagree with each other. If you'd hand-written all six, a typo in one would give you a < b and a >= b both true, and you'd be debugging for an hour.

5. Pre- vs post-increment
A genuine C++ oddity worth knowing cold, since 42 exams love it:


bigint& operator++()    { ... }  // ++b  — prefix
bigint  operator++(int) { ... }  // b++  — postfix
The int parameter is fake. It's never passed a value and has no name. It exists purely because the language needed some way to distinguish two functions that would otherwise have identical signatures. It's a syntactic marker, not data.

And the return types differ for a real reason: ++b increments then hands you back the object itself (a reference — cheap). b++ must hand you back the value as it was before, which no longer exists anywhere — so it has to copy it first (line 26: bigint t(*this);), increment, then return the copy by value. That's why post-increment is the more expensive one, and why C++ programmers habitually write ++i in loops.

6. Orthodox Canonical Form — and why it's boring here
42 requires four members on every class:


bigint();                             // default constructor
bigint(const bigint& o);              // copy constructor      — build a new one from an existing one
bigint& operator=(const bigint& o);   // copy assignment       — overwrite an existing one
~bigint();                            // destructor
The real principle is the Rule of Three: if your class manages a resource (raw new, a file handle, a socket), you need all three of copy-ctor / assignment / destructor, because the compiler's defaults would copy the pointer and you'd get a double-free.

Here, std::string already handles its own memory. So all four of mine are one-liners, and honestly the compiler-generated versions would be identical. I wrote them out because the exam wants to see them — but understand that they're ceremony in this class and life-or-death in a class holding a char*. Knowing which situation you're in is the actual skill.

7. The invisible conversion machinery
This is the piece that most often feels like magic when you're rusty. Look at line 29:


friend bigint operator<<(bigint a, const bigint& k);
It takes a bigint count. Yet the subject's main writes b << 10, passing an int. Why does that compile?

Because bigint.hpp:14 is a constructor taking a single int and is not marked explicit. Any single-argument non-explicit constructor doubles as an implicit conversion: wherever a bigint is expected and an int is supplied, the compiler silently builds a temporary. So b << 10 becomes b << bigint(10). One constructor bought me + 42, < 100, == 42, and >>= 2 for free.

The flip side is on line 15, where the string constructor is explicit — that switches the automatic conversion off, so you must write bigint(str) deliberately. I did that to prevent the compiler from getting creative with string literals near std::cout <<. That's the tension to remember: implicit conversions are convenient and occasionally dangerous, so the default advice is mark single-argument constructors explicit unless you specifically want the convenience.

One more, from the same main:


const bigint a(42);
std::cout << a + b;
a is const. Reading it must not require permission to modify it — so every operation on it takes const bigint& and, if it were a member, would be marked const. That's const-correctness: it's not decoration, it's what lets a read-only object still be usable. If you forget one const, this main stops compiling, which is exactly the trap the subject is setting.

If you want to check whether it actually landed, the best exercise is subtraction — the subject doesn't ask for it, so there's no answer to peek at. Write operator-= by hand (borrow instead of carry, and re-strip leading zeros afterward since 1000 - 999 leaves "0001", which breaks the invariant), then see if you can derive operator- from it in one line. If that one line comes naturally, you've got section 4, which is the transferable part.

Tell me which section feels shakiest and I'll go deeper there — sections 3 and 7 are the ones that usually need a second pass.