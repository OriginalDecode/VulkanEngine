#include <cstdio>
#include "gtest/gtest.h"

#include "Core/math/Vector4.h"
#include "Core/math/Vector3.h"
#include "Core/math/Vector2.h"

/*
	different macros for unit tests

	EXPECT_EQ // equal
	EXPECT_NE // not equal
	EXPECT_LT // less than
	EXPECT_LE // lesser or equal than
	EXPECT_GT // greater than
	EXPECT_GE // greater or equal than

	Expect does not assume fatal
	
	ASSERT_TRUE //fatal
	ASSERT_FALSE //fatal


*/

TEST( Vector4, Length )
{
	constexpr float a = 193.f, b = 284.f, c = 321.f, d = 461.f;
	constexpr float constexpr_result = ( a * a ) + ( b * b ) + ( c * c ) + ( d * d );
	Core::Vector4<float> vector( a, b, c, d );
	ASSERT_EQ( vector.Length(), sqrtf( constexpr_result ) );
}

TEST( Vector4, Length2 )
{
	constexpr float a = 193.f, b = 284.f, c = 321.f, d = 461.f;
	constexpr float constexpr_result = ( a * a ) + ( b * b ) + ( c * c ) + ( d * d );
	Core::Vector4<float> vector( a, b, c, d );
	ASSERT_EQ( vector.Length2(), constexpr_result );
}

GTEST_API_ int main( int argc, char** argv )
{
	printf( "Running main() from %s\n", __FILE__ );
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}