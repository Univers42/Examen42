#include "bigint.hpp"

/**
count doesn't count characters. it parses the digit string back into a machine integet. Think of it as
`to_size_t` / `atoi`. The reason it has to eisti:

The shift amount `k` is itself a `bigint` , so its vlaue only exists as the string `k.s`. But the operations we need in the body take a real `size_t`

```bash
this->s.append(count(k.s), '0');		//append(size_t n, char c)
s.erase(s.size() - n);					// needs size_t
```

we can't append 3 zeros - we need the number 3. So count decodes the digits

Trace bigint(42) << bigint(3)
step value
k.s	3
count("3")
*/
static size_t count(const std::string& s) {
	if (s.size() > 9)
		return ~size_t(0);	// max value.
	size_t k = 0;
	for (size_t i = 0; i < s.size(); ++i)
		k = k * 10 + size_t(s[i] - '0');
	return k;
}

/**
Schoolbook addition, right to left, split into two phases so neither one pays
for the other's checks.

	lo   = number of digit positions where both operands have a digit
	n    = final width of *this before a possible carry out

If `o` is wider, we don't pad with '0' and then add those digits: we copy o's
high digits straight in. They are already the correct result there, up to a
carry that phase 2 will ripple through. This is the one change that is worth a
large constant factor: padding forces a rescan of the whole padded string, so
`small += big` went from O(width of big) real additions to O(width of small).

	phase 1  [0, lo)      real addition, no bounds test per digit
	phase 2  [lo, n)      carry ripple only: '9' -> '0', else ++ and stop

Three details worth naming:
  - d is at most 9 + 9 + 1 = 19, so `d % 10` / `d / 10` are overkill. The
	normalisation is written branchlessly (`carry = (d >= 10)`) on purpose: a
	`if (d >= 10)` here is a data dependent branch that the predictor cannot
	learn on real inputs, and it measured ~4x slower on long operands.
  - sq/bq walk raw pointers instead of indexing s[]. Because a char write may
	alias anything, indexing forces the compiler to reload s.data() and
	s.size() on every digit; at -O0 each s[j] is also a real un-inlined call.
  - self assignment (a += a) is safe: for a given i, sq and bq are the same
	cell, and both are read before the write.
*/
bigint& bigint::operator+=(const bigint& o) {
	const std::string& b = o.s;
	const size_t nb = b.size();
	size_t lo = this->s.size();
	if (lo < nb)
		this->s.insert(0, b, 0, nb - lo);	// graft o's high digits, no '0' padding
	else
		lo = nb;
	const size_t n = this->s.size();
	char* sq = &this->s[0] + n - 1;
	const char* bq = b.data() + nb - 1;
	int carry = 0;
	size_t i = 0;
	for (; i < lo; ++i, --sq, --bq) {
		int d = (*sq - '0') + (*bq - '0') + carry;
		carry = (d >= 10);
		*sq = char('0' + d - 10 * carry);
	}
	for (; carry && i < n; ++i, --sq) {
		if (*sq == '9')
			*sq = '0';
		else {
			++*sq;
			carry = 0;
		}
	}
	if (carry)
		this->s.insert(this->s.begin(), '1');
	return *this;
}

/**
effect: append n zeros(That test ran against a modified copy in the scratchpad; your real header is untouched.)


multiply by 10^n
*/
bigint& bigint::operator<<=(const bigint& k) {
	if (this->s != "0")
		this->s.append(count(k.s), '0');
	return *this;
}

/**
effect erase last n digits
arithmetic meaning they divide by 10^n
*/
bigint& bigint::operator>>=(const bigint& k) {
	size_t n = count(k.s);
	if (n >= this->s.size())
		s = "0";
	else
		s.erase(s.size() - n);
	return *this;
}
