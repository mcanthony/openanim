#include "Skeleton.h"

#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

namespace openanim {

Skeleton::Joint::Joint(const std::string& name, int parent, const Children<Joint, Skeleton>& chld, Skeleton* Skeleton) : m_name(name), m_parent(parent), m_children(chld), m_skeleton(Skeleton) {
}

const std::string& Skeleton::Joint::name() const {
	return m_name;
}

std::size_t Skeleton::Joint::index() const {
	return m_skeleton->indexOf(*this);
}

Children<Skeleton::Joint, Skeleton>& Skeleton::Joint::children() {
	return m_children;
}

const Children<Skeleton::Joint, Skeleton>& Skeleton::Joint::children() const {
	return m_children;
}

bool Skeleton::Joint::hasParent() const {
	return m_parent >= 0;
}

Skeleton::Joint& Skeleton::Joint::parent() {
	assert(hasParent());
	return (*m_skeleton)[m_parent];
}

const Skeleton::Joint& Skeleton::Joint::parent() const {
	assert(hasParent());
	return (*m_skeleton)[m_parent];
}

////////

Skeleton::Skeleton() {
}

Skeleton::Skeleton(const Skeleton& h) : m_joints(h.m_joints) {
	for(auto& j : m_joints)
		j.children().m_joints = this;
}

Skeleton& Skeleton::operator = (const Skeleton& h) {
	m_joints = h.m_joints;

	for(auto& j : m_joints)
		j.children().m_joints = this;

	return *this;
}

Skeleton::Skeleton(Skeleton&& h) : m_joints(std::move(h.m_joints)) {
	for(auto& j : m_joints)
		j.children().m_joints = this;
}

Skeleton& Skeleton::operator = (Skeleton&& h) {
	m_joints = std::move(h.m_joints);

	for(auto& j : m_joints)
		j.children().m_joints = this;

	return *this;
}

Skeleton::Joint& Skeleton::operator[](std::size_t index) {
	assert(index < m_joints.size());
	return m_joints[index];
}

const Skeleton::Joint& Skeleton::operator[](std::size_t index) const {
	assert(index < m_joints.size());
	return m_joints[index];
}

bool Skeleton::empty() const {
	return m_joints.empty();
}

size_t Skeleton::size() const {
	return m_joints.size();
}

std::size_t Skeleton::indexOf(const Joint& j) const {
	assert(j.children().m_joints == this);

	return (&j - &(*m_joints.begin()));
}

void Skeleton::addRoot(const std::string& name) {
	if(empty())
		// create a single root joint, with children "behind the end"
		m_joints.push_back(Joint(name, -1, Children<Joint, Skeleton>(1, 1, *this), this));
	else {
		// add a new joint at the beginning
		m_joints.insert(m_joints.begin(), Joint(name, -1, Children<Joint, Skeleton>(1, 2, *this), this));
		// and update the children indices of all following joints
		for(auto it = m_joints.begin()+1; it != m_joints.end(); ++it) {
			++it->children().m_begin;
			++it->children().m_end;

			++it->m_parent;
		}
	}
}

std::size_t Skeleton::addChild(const Joint& j, const std::string& name) {
	assert(j.children().m_joints == this && "input joint has to be part of the current Skeleton!");

	// find the joint's last child - the position we'll be inserting into
	const std::size_t lastChild = j.children().m_end;
	assert(lastChild > indexOf(j));

	// update all children indices larger than the inserted number
	const std::size_t currentIndex = indexOf(j);
	for(std::size_t ji = 0; ji < m_joints.size(); ++ji) {
		Joint& i = m_joints[ji];

		if(ji == currentIndex) {
			// update the m_end of joint's children to include this new joint
			++i.children().m_end;
			assert(lastChild == i.children().m_end-1);
		}
		else if(i.children().m_begin > lastChild) {
			++i.children().m_begin;
			++i.children().m_end;
		}
		else if((i.children().m_begin == lastChild) && (ji > currentIndex)) {
			++i.children().m_begin;
			++i.children().m_end;
		}

		if(i.m_parent >= (int)lastChild)
			++i.m_parent;
	}

	// figure out the new children position
	unsigned childPos = lastChild+1;
	while((childPos-1 < m_joints.size()) && (m_joints[childPos-1].m_parent <= (int)lastChild))
		++childPos;

	// and insert the joint
	m_joints.insert(m_joints.begin() + lastChild, Joint(name, currentIndex, Children<Joint, Skeleton>(childPos, childPos, *this), this));

	// and return the index of newly inserted joint
	return lastChild;
}

Skeleton::const_iterator Skeleton::begin() const {
	return m_joints.begin();
}

Skeleton::const_iterator Skeleton::end() const {
	return m_joints.end();
}

Skeleton::iterator Skeleton::begin() {
	return m_joints.begin();
}

Skeleton::iterator Skeleton::end() {
	return m_joints.end();
}


}
