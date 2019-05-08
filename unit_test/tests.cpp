#include <cstdio>
#include "gtest/gtest.h"

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

TEST( OverarchingTest, Testname )
{
	ASSERT_EQ( 1, 0 );
}

GTEST_API_ int main( int argc, char** argv )
{
	printf( "Running main() from %s\n", __FILE__ );
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}