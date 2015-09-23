#include "Skeleton.h"

#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

namespace openanim {

Skeleton::Joint::Joint(std::size_t id, Skeleton* skel) : m_id(id), m_skeleton(skel) {
}

const std::string& Skeleton::Joint::name() const {
	assert(m_skeleton != NULL);
	assert(m_id < m_skeleton->size());
	return (*m_skeleton->m_hierarchy)[m_id].name;
}

std::size_t Skeleton::Joint::index() const {
	return m_id;
}

Children<Skeleton::Joint, Skeleton> Skeleton::Joint::children() {
	assert(m_skeleton != NULL);
	auto& j = (*m_skeleton->m_hierarchy)[m_id];
	return Children<Skeleton::Joint, Skeleton>(j.children_begin, j.children_end, *m_skeleton);
}

const Children<Skeleton::Joint, Skeleton> Skeleton::Joint::children() const {
	assert(m_skeleton != NULL);
	auto& j = (*m_skeleton->m_hierarchy)[m_id];
	return Children<Skeleton::Joint, Skeleton>(j.children_begin, j.children_end, *m_skeleton);
}

bool Skeleton::Joint::hasParent() const {
	assert(m_skeleton != NULL);
	assert(m_id < m_skeleton->size());
	return (*m_skeleton->m_hierarchy)[m_id].parent >= 0;
}

Skeleton::Joint& Skeleton::Joint::parent() {
	assert(m_skeleton != NULL);
	assert(m_id < m_skeleton->size());
	assert(hasParent());
	return (*m_skeleton)[(*m_skeleton->m_hierarchy)[m_id].parent];
}

const Skeleton::Joint& Skeleton::Joint::parent() const {
	assert(m_skeleton != NULL);
	assert(m_id < m_skeleton->size());
	assert(hasParent());
	return (*m_skeleton)[(*m_skeleton->m_hierarchy)[m_id].parent];
}

////////

Skeleton::Skeleton() : m_hierarchy(new Hierarchy()) {
}

Skeleton::Skeleton(const Skeleton& h) : m_joints(h.m_joints), m_hierarchy(h.m_hierarchy) {
	for(auto& j : m_joints)
		j.m_skeleton = this;
}

Skeleton& Skeleton::operator = (const Skeleton& h) {
	m_hierarchy = h.m_hierarchy;
	m_joints = h.m_joints;

	for(auto& j : m_joints)
		j.m_skeleton = this;

	return *this;
}

Skeleton::Skeleton(Skeleton&& h) : m_joints(std::move(h.m_joints)), m_hierarchy(std::move(h.m_hierarchy)) {
	for(auto& j : m_joints)
		j.m_skeleton = this;
}

Skeleton& Skeleton::operator = (Skeleton&& h) {
	m_joints = std::move(h.m_joints);
	m_hierarchy = std::move(h.m_hierarchy);

	for(auto& j : m_joints)
		j.m_skeleton = this;

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
	// changing the hierarchy means the result is no longer compatible with other instances sharing the same
	// hierarchy instance
	m_hierarchy = std::shared_ptr<Hierarchy>(new Hierarchy(*m_hierarchy));

	// create a single root joint, with children "behind the end"
	m_hierarchy->addRoot(name);

	// and just add a joint to the hierarchy - doesn't really matter where for now
	m_joints.push_back(Joint(m_joints.size(), this));
}

std::size_t Skeleton::addChild(const Joint& j, const std::string& name) {
	assert(j.m_skeleton == this && "input joint has to be part of the current Skeleton!");

	// changing the hierarchy means the result is no longer compatible with other instances sharing the same
	// hierarchy instance
	m_hierarchy = std::shared_ptr<Hierarchy>(new Hierarchy(*m_hierarchy));

	// add a child
	std::size_t index = m_hierarchy->addChild((*m_hierarchy)[j.m_id], name);

	// and just add a joint to the hierarchy - doesn't really matter where for now
	m_joints.push_back(Joint(m_joints.size(), this));

	return index;
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

bool Skeleton::isCompatibleWith(const Skeleton& s) const {
	return m_hierarchy == s.m_hierarchy;
}

}
