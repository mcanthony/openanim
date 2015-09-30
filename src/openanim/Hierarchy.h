#pragma once

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>

#include "Children.h"

namespace openanim {

/// Hierarchy class describes a hierarchy of transformations (skeleton) as a flat array of named Joint
/// objects. It guarantees that the index of parent joint is always lower than index of children joints,
/// allowing for replacing recursive operations (e.g, world-to-local conversion) to simple iterations.
/// The internal representation of joint data might change in the future (the interface will probably not).
class Hierarchy {
	public:
		struct Item {
			std::string name;
			int parent;
			std::size_t children_begin, children_end;
		};

		const Item& operator[](std::size_t index) const;

		bool empty() const;
		size_t size() const;

		void addRoot(const std::string& name);
		std::size_t addChild(const Item& i, const std::string& name);

		typedef std::vector<Item>::const_iterator const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		typedef std::vector<Item>::iterator iterator;
		iterator begin();
		iterator end();

	protected:
	private:
		std::size_t indexOf(const Item& j) const;

		std::vector<Item> m_items;
};

}
