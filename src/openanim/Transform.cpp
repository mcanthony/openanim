#include "Transform.h"

namespace openanim {

Transform::Transform() :
	// vector has to be initialised explicitly, but quaternion doesn't
	translation(0,0,0) {
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
	return Transform(
		// what a pretty inconsistency in OpenEXR
		t.rotation * rotation,
		translation*t.rotation + t.translation
	);
}

Transform& Transform::operator *= (const Transform& t) {
	// what a pretty inconsistency in OpenEXR
	rotation = t.rotation * rotation;
	translation = translation*t.rotation + t.translation;

	return *this;
}

std::ostream& operator << (std::ostream& out, const Transform& tr) {
	out << "(" << tr.rotation << "), (" << tr.translation << ")";

	return out;
}

};
