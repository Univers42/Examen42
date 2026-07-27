#ifndef SEARCHABLE_ARRAY_BAG_HPP
#define SEARCHABLE_ARRAY_BAG_HPP
#include "array_bag.hpp"
#include "searchable_bag.hpp"

class searchable_array_bag : public array_bag, public searchable_bag {
public:
	searchable_array_bag() {}
	searchable_array_bag(const searchable_array_bag &o) : array_bag(o) {}
	searchable_array_bag &operator=(const searchable_array_bag &o) { array_bag::operator=(o); return *this; }
	~searchable_array_bag() {}
	bool has(int v) const {
		for (int i = 0; i < size; i++)
			if (data[i] == v)
				return true;
		return false;
	}
};

#endif
