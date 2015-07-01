#include "Transform.h"

namespace openanim {

Transform::Transform() {
}

Transform::Transform(const Imath::V3f& tr) : translation(tr) {
}

Transform::Transform(const Imath::Quatf& rot, const Imath::V3f& tr) :
	translation(tr), rotation(rot) {

}

const Imath::M44f Transform::toMatrix44() const {
	Imath::M44f result = rotation.toMatrix44();
	result[3][0] = translation[0];
	result[3][1] = translation[1];
	result[3][2] = translation[2];

	return result;
}

const Transform Transform::operator * (const Transform& t) const {
	return *this;
}

Transform& Transform::operator *= (const Transform& t) {
	return *this;
}

std::ostream& operator << (std::ostream& out, const Transform& tr) {
	out << "(" << tr.rotation << "), (" << tr.translation << ")";

	return out;
}

};
