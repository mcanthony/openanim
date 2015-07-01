#include "openanim/Transform.h"

#include <ImathEuler.h>

#include <boost/test/unit_test.hpp>

using std::cout;
using std::endl;

const float compareMatrices(const Imath::M44f& m1, const Imath::M44f& m2) {
	// cout << m1 << endl << m2 << endl << endl;

	float result = 0.0f;
	for(unsigned a=0;a<4;++a)
		for(unsigned b=0;b<4;++b)
			result += std::abs(m1[a][b] - m2[a][b]);
	return result;
}

namespace {
	static const float EPS = 1e-4f;
}

/////////////
// tests mainly the correspondence between the transformation class
// and a 4x4 matrix.

BOOST_AUTO_TEST_CASE(transform_init) {
	std::vector<std::pair<openanim::Transform, Imath::M44f>> data = {
		{
			openanim::Transform(),
			Imath::M44f(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1)
		},
		{
			openanim::Transform(Imath::V3f(1,2,3)),
			Imath::M44f(1,0,0,0, 0,1,0,0, 0,0,1,0, 1,2,3,1)
		},
		{
			openanim::Transform(Imath::Quatf(0.326096f, -0.0849528, -0.190674, 0.922001)),
			Imath::M44f(
				-0.772889 ,  0.633718, -0.0322979, 0,
				-0.568925 , -0.71461 , -0.407009 , 0,
				-0.28101  , -0.296198,  0.912853 , 0,
				0,0,0,1)
		},
		{
			openanim::Transform(Imath::Quatf(0.326096f, -0.0849528, -0.190674, 0.922001), Imath::V3f(3,4,5)),
			Imath::M44f(
				-0.772889 ,  0.633718, -0.0322979, 0,
				-0.568925 , -0.71461 , -0.407009 , 0,
				-0.28101  , -0.296198,  0.912853 , 0,
				3,4,5,1)
		}
	};

	for(auto& i : data)
		BOOST_REQUIRE_SMALL(
			compareMatrices(
				i.first.toMatrix44(),
				i.second
			),
			EPS
		);
}

////////////////

namespace {
	const Imath::M44f toMatrix44(const std::pair<Imath::Eulerf, Imath::V3f>& data) {
		Imath::M44f result = data.first.toMatrix44();
		result[3][0] = data.second[0];
		result[3][1] = data.second[1];
		result[3][2] = data.second[2];

		return result;
	}
}

BOOST_AUTO_TEST_CASE(transform_multiplication) {
	std::vector<std::pair<
		std::pair<Imath::Eulerf, Imath::V3f>,
		std::pair<Imath::Eulerf, Imath::V3f>
	>> data = {
		{
			{
				Imath::Eulerf(-25,20,36),
				Imath::V3f(0,0,0)
			},
			{
				Imath::Eulerf(89,-93,63),
				Imath::V3f(0,0,0)
			}
		},
		{
			{
				Imath::Eulerf(-25,20,36),
				Imath::V3f(1,2,3)
			},
			{
				Imath::Eulerf(89,-93,63),
				Imath::V3f(0,0,0)
			}
		},
		{
			{
				Imath::Eulerf(-25,20,36),
				Imath::V3f(1,2,3)
			},
			{
				Imath::Eulerf(89,-93,63),
				Imath::V3f(7,6,5)
			}
		}
	};

	for(auto& i : data)
		BOOST_REQUIRE_SMALL(
			compareMatrices(
				(openanim::Transform(i.first.first.toQuat(), i.first.second) * openanim::Transform(i.second.first.toQuat(), i.second.second)).toMatrix44(),
				toMatrix44(i.first) * toMatrix44(i.second)
			),
			EPS
		);

	for(auto& i : data) {
		openanim::Transform t(i.first.first.toQuat(), i.first.second);
		t *= openanim::Transform(i.second.first.toQuat(), i.second.second);

		BOOST_REQUIRE_SMALL(
			compareMatrices(
				t.toMatrix44(),
				toMatrix44(i.first) * toMatrix44(i.second)
			),
			EPS
		);
	}
}
