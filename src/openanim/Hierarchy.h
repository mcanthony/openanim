#pragma once

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>

namespace openanim {

/// Hierarchy class describes a hierarchy of transformations (skeleton) as a flat array of named Joint
/// objects. It guarantees that the index of parent joint is always lower than index of children joints,
/// allowing for replacing recursive operations (e.g, world-to-local conversion) to simple iterations.
/// The internal representation of joint data might change in the future (the interface will probably not).
class Hierarchy {
	public:
		class Joint;

		/// an encapsulation of an iterator range for the children of a particular joint.
		/// All joints are stored in a flat array, and ths class just returns two iterators
		/// with the right offsets.
		class Children {
			public:
				typedef std::vector<Joint>::const_iterator const_iterator;
				const_iterator begin() const;
				const_iterator end() const;

				typedef std::vector<Joint>::iterator iterator;
				iterator begin();
				iterator end();

				bool valid() const;
				bool empty() const;

				std::size_t size() const;
				Joint& operator[](std::size_t index);
				const Joint& operator[](std::size_t index) const;

			private:
				Children();
				Children(std::size_t begin, std::size_t end, std::vector<Joint>& joints);

				std::size_t m_begin, m_end;
				std::vector<Joint>* m_joints;

			friend class Hierarchy;
		};

		/// a single joint and its children.
		/// The joint instance behaves like an iterator - it is just a weak
		/// reference to the Hierarchy structure and cannot exist on its own.
		class Joint {
			public:
				const std::string& name() const;
				std::size_t index() const;

				Children& children();
				const Children& children() const;

				bool hasParent() const;
				Joint& parent();
				const Joint& parent() const;

			private:
				Joint(const std::string& name, int parent, const Children& chld, Hierarchy* hierarchy);

				std::string m_name;

				int m_parent;
				Children m_children;

				Hierarchy* m_hierarchy;

			friend class Hierarchy;
		};

		Hierarchy();
		Hierarchy(const Hierarchy& h);
		Hierarchy& operator = (const Hierarchy& h);
		Hierarchy(Hierarchy&& h);
		Hierarchy& operator = (Hierarchy&& h);

		Joint& operator[](std::size_t index);
		const Joint& operator[](std::size_t index) const;
		std::size_t indexOf(const Joint& j) const;

		bool empty() const;
		size_t size() const;

		void addRoot(const std::string& name);
		std::size_t addChild(const Joint& j, const std::string& name);

		typedef std::vector<Joint>::const_iterator const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		typedef std::vector<Joint>::iterator iterator;
		iterator begin();
		iterator end();

	protected:
	private:
		std::vector<Joint> m_joints;
};

}