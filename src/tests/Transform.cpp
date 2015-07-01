#include "openanim/Transform.h"

#include <boost/test/unit_test.hpp>

using std::cout;
using std::endl;

const float compareMatrices(const Imath::M44f& m1, const Imath::M44f& m2) {
	float result = 0.0f;
	for(unsigned a=0;a<4;++a)
		for(unsigned b=0;b<4;++b)
			result += std::abs(m1[a][b] - m2[a][b]);
	return result;
}

namespace {
	static const float EPS = 1e-4f;
}

BOOST_AUTO_TEST_CASE(transform_init) {
	{
		openanim::Transform t;

		BOOST_REQUIRE_SMALL(
			compareMatrices(
				t.toMatrix44(),
				Imath::M44f(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1)
			),
			EPS
		);
	}

	{
		openanim::Transform t(Imath::V3f(1,2,3));

		BOOST_REQUIRE_SMALL(
			compareMatrices(
				t.toMatrix44(),
				Imath::M44f(1,0,0,0, 0,1,0,0, 0,0,1,0, 1,2,3,1)
			),
			EPS
		);
	}

	{
		openanim::Transform t(Imath::Quatf(0.326096f, -0.0849528, -0.190674, 0.922001));

		BOOST_REQUIRE_SMALL(
			compareMatrices(
				t.toMatrix44(),
				Imath::M44f(
					-0.772889 ,  0.633718, -0.0322979, 0,
					-0.568925 , -0.71461 , -0.407009 , 0,
					-0.28101  , -0.296198,  0.912853 , 0,
					0,0,0,1)
			),
			EPS
		);
	}

	{
		openanim::Transform t(Imath::Quatf(0.326096f, -0.0849528, -0.190674, 0.922001), Imath::V3f(3,4,5));

		BOOST_REQUIRE_SMALL(
			compareMatrices(
				t.toMatrix44(),
				Imath::M44f(
					-0.772889 ,  0.633718, -0.0322979, 0,
					-0.568925 , -0.71461 , -0.407009 , 0,
					-0.28101  , -0.296198,  0.912853 , 0,
					3,4,5,1)
			),
			EPS
		);
	}
}
