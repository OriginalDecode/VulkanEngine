#define UNIT_TEST

#include <cstdio>
#include "gtest/gtest.h"

#include "Core/math/Vector4.h"
#include "Core/math/Vector3.h"
#include "Core/math/Vector2.h"
#include "Core/containers/Array.h"

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

TEST( Vector4, dot )
{
	constexpr float a = 193.f, b = 284.f, c = 321.f, d = 461.f;
	constexpr float e = 913.f, f = 378.f, g = 245.f, h = 560.f;
	constexpr float dot_result = ( a * e ) + ( b * f ) + ( c * g ) + ( d * h );

	Core::Vector4<float> first( a, b, c, d );
	Core::Vector4<float> second( e, f, g, h );

	ASSERT_EQ( Core::Dot( first, second ), dot_result );
}

TEST( Array, create )
{
	Core::Array<float> array;
	ASSERT_TRUE( array.Size() != array.Capacity() );
	ASSERT_TRUE( array.Size() == 0 );
	ASSERT_TRUE( array.Capacity() == 20 ); // if the default capacity ever changes this test will break
}

TEST( Array, addOne )
{
	Core::Array<float> array;
	array.Add( 1.f );
	ASSERT_TRUE( array.Size() == 1 );
}

TEST( Array, fill )
{
	Core::Array<float> array;
	for( int i = 0; i < array.Capacity(); i++ )
		array.Add( 1.f );

	ASSERT_EQ( array.Size(), array.Capacity() );
}

TEST( Array, Copy )
{
	Core::Array<float> array;
	for( int i = 0; i < array.Capacity(); i++ )
		array.Add( 1.f );

	Core::Array<float> array2( array );

	ASSERT_TRUE( array2.m_Data != nullptr );
	ASSERT_TRUE( array.m_Data != nullptr );
	ASSERT_NE( array.m_Data, array2.m_Data );
}

GTEST_API_ int main( int argc, char** argv )
{
	printf( "Running main() from %s\n", __FILE__ );
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}