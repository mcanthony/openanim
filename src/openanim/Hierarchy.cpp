#include "Hierarchy.h"

#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

namespace openanim {

Hierarchy::Children::Children() : m_begin(0), m_end(0), m_joints(NULL) {
}

Hierarchy::Children::Children(std::size_t begin, std::size_t end, std::vector<Joint>& joints) : m_begin(begin), m_end(end), m_joints(&joints) {
}

Hierarchy::Children::const_iterator Hierarchy::Children::begin() const {
	assert(valid());
	return m_joints->begin() + m_begin;
}

Hierarchy::Children::const_iterator Hierarchy::Children::end() const {
	assert(valid());
	return m_joints->begin() + m_end;
}

Hierarchy::Children::iterator Hierarchy::Children::begin() {
	assert(valid());
	return m_joints->begin() + m_begin;
}

Hierarchy::Children::iterator Hierarchy::Children::end() {
	assert(valid());
	return m_joints->end() + m_end;
}

bool Hierarchy::Children::valid() const {
	return m_joints != NULL;
}

bool Hierarchy::Children::empty() const {
	return (m_joints == NULL) || (m_begin == m_end);
}

std::size_t Hierarchy::Children::size() const {
	assert(valid());
	return m_end - m_begin;
}

Hierarchy::Joint& Hierarchy::Children::operator[](std::size_t index) {
	assert(valid());
	assert(index < m_joints->size());
	return (*m_joints)[index];
}

const Hierarchy::Joint& Hierarchy::Children::operator[](std::size_t index) const {
	assert(valid());
	assert(index < m_joints->size());
	return (*m_joints)[index];
}

////////

Hierarchy::Joint::Joint(const std::string& name, int parent, const Children& chld, Hierarchy* hierarchy) : m_name(name), m_parent(parent), m_children(chld), m_hierarchy(hierarchy) {
}

const std::string& Hierarchy::Joint::name() const {
	return m_name;
}

std::size_t Hierarchy::Joint::index() const {
	return m_hierarchy->indexOf(*this);
}

Hierarchy::Children& Hierarchy::Joint::children() {
	return m_children;
}

const Hierarchy::Children& Hierarchy::Joint::children() const {
	return m_children;
}

bool Hierarchy::Joint::hasParent() const {
	return m_parent >= 0;
}

Hierarchy::Joint& Hierarchy::Joint::parent() {
	assert(hasParent());
	return (*m_hierarchy)[m_parent];
}

const Hierarchy::Joint& Hierarchy::Joint::parent() const {
	assert(hasParent());
	return (*m_hierarchy)[m_parent];
}

////////

Hierarchy::Hierarchy() {
}

Hierarchy::Hierarchy(const Hierarchy& h) : m_joints(h.m_joints) {
	for(auto& j : m_joints)
		j.children().m_joints = &m_joints;
}

Hierarchy& Hierarchy::operator = (const Hierarchy& h) {
	m_joints = h.m_joints;

	for(auto& j : m_joints)
		j.children().m_joints = &m_joints;

	return *this;
}

Hierarchy::Hierarchy(Hierarchy&& h) : m_joints(std::move(h.m_joints)) {
	for(auto& j : m_joints)
		j.children().m_joints = &m_joints;
}

Hierarchy& Hierarchy::operator = (Hierarchy&& h) {
	m_joints = std::move(h.m_joints);

	for(auto& j : m_joints)
		j.children().m_joints = &m_joints;

	return *this;
}

Hierarchy::Joint& Hierarchy::operator[](std::size_t index) {
	assert(index < m_joints.size());
	return m_joints[index];
}

const Hierarchy::Joint& Hierarchy::operator[](std::size_t index) const {
	assert(index < m_joints.size());
	return m_joints[index];
}

bool Hierarchy::empty() const {
	return m_joints.empty();
}

size_t Hierarchy::size() const {
	return m_joints.size();
}

std::size_t Hierarchy::indexOf(const Joint& j) const {
	assert(j.children().m_joints == &m_joints);

	return (&j - &(*m_joints.begin()));
}

void Hierarchy::addRoot(const std::string& name) {
	if(empty())
		// create a single root joint, with children "behind the end"
		m_joints.push_back(Joint(name, -1, Children(1, 1, m_joints), this));
	else {
		// add a new joint at the beginning
		m_joints.insert(m_joints.begin(), Joint(name, -1, Children(1, 2, m_joints), this));
		// and update the children indices of all following joints
		for(auto it = m_joints.begin()+1; it != m_joints.end(); ++it) {
			++it->children().m_begin;
			++it->children().m_end;

			++it->m_parent;
		}
	}
}

std::size_t Hierarchy::addChild(const Joint& j, const std::string& name) {
	assert(j.children().m_joints == &m_joints && "input joint has to be part of the current hierarchy!");

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
	m_joints.insert(m_joints.begin() + lastChild, Joint(name, currentIndex, Children(childPos, childPos, m_joints), this));

	// and return the index of newly inserted joint
	return lastChild;
}

Hierarchy::const_iterator Hierarchy::begin() const {
	return m_joints.begin();
}

Hierarchy::const_iterator Hierarchy::end() const {
	return m_joints.end();
}

Hierarchy::iterator Hierarchy::begin() {
	return m_joints.begin();
}

Hierarchy::iterator Hierarchy::end() {
	return m_joints.end();
}


}
