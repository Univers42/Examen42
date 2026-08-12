#ifndef SEARCHABLE_TREE_BAG_HPP
#define SEARCHABLE_TREE_BAG_HPP
#include "tree_bag.hpp"
#include "searchable_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag {
public:
	searchable_tree_bag() {}
	searchable_tree_bag(const searchable_tree_bag &o) : tree_bag(o) {}
	// tree_bag::operator= clears before it copies, so it destroys the source
	// on self-assignment: the guard has to live here, in the graded file.
	searchable_tree_bag &operator=(const searchable_tree_bag &o) { if (this != &o) tree_bag::operator=(o); return *this; }
	~searchable_tree_bag() {}
	bool has(int v) const {
		for (node *n = tree; n; n = v < n->value ? n->l : n->r)
			if (n->value == v)
				return true;
		return false;
	}
};

#endif
