#pragma once

#include <iostream>

#include <ImathQuat.h>
#include <ImathVec.h>

namespace openanim {

/// a single rigid body transformation
struct Transform {
	Imath::V3f translation;
	Imath::Quatf rotation;

	/// initialises the transformation to identity
	Transform();
	/// initialises only translation part
	Transform(const Imath::V3f& tr);
	/// initialises both translation and rotation
	Transform(const Imath::Quatf& rot, const Imath::V3f& tr = Imath::V3f(0,0,0));

	const Imath::M44f toMatrix44() const;

	const Transform operator * (const Transform& t) const;
	Transform& operator *= (const Transform& t);
};

std::ostream& operator << (std::ostream& out, const Transform& tr);

};
