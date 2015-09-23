#pragma once

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>

#include "Hierarchy.h"

namespace openanim {

class Skeleton {
	public:
		class Joint;

		typedef std::vector<Joint>::const_iterator const_iterator;
		typedef std::vector<Joint>::iterator iterator;

		/// a single joint and its children.
		/// The joint instance behaves like an iterator - it is just a weak
		/// reference to the Hierarchy structure and cannot exist on its own.
		class Joint {
			public:
				const std::string& name() const;
				std::size_t index() const;

				Children<Joint, Skeleton> children();
				const Children<Joint, Skeleton> children() const;

				bool hasParent() const;
				Joint& parent();
				const Joint& parent() const;

			private:
				Joint(std::size_t id, Skeleton* skel);

				std::size_t m_id;
				Skeleton* m_skeleton;

			friend class Skeleton;
		};

		Skeleton();
		Skeleton(const Skeleton& h);
		Skeleton& operator = (const Skeleton& h);
		Skeleton(Skeleton&& h);
		Skeleton& operator = (Skeleton&& h);

		Joint& operator[](std::size_t index);
		const Joint& operator[](std::size_t index) const;
		std::size_t indexOf(const Joint& j) const;

		bool empty() const;
		size_t size() const;

		void addRoot(const std::string& name);
		std::size_t addChild(const Joint& j, const std::string& name);

		const_iterator begin() const;
		const_iterator end() const;

		iterator begin();
		iterator end();

	protected:
	private:
		// exists only so I can return references to joints, not instances
		std::vector<Joint> m_joints;
		// stores the hierachy of joints
		Hierarchy m_hierarchy;
};

}
