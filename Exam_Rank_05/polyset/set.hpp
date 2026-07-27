#ifndef SET_HPP
#define SET_HPP
#include "searchable_bag.hpp"

class set {
	searchable_bag *_bag;
public:
	set(searchable_bag &b) : _bag(&b) {}
	set(const set &o) : _bag(o._bag) {}
	set &operator=(const set &o) { _bag = o._bag; return *this; }
	~set() {}
	void insert(int v) { if (!_bag->has(v)) _bag->insert(v); }
	void insert(int *arr, int n) { for (int i = 0; i < n; i++) insert(arr[i]); }
	bool has(int v) const { return _bag->has(v); }
	void print() const { _bag->print(); }
	void clear() { _bag->clear(); }
	searchable_bag &get_bag() const { return *_bag; }
};

#endif
