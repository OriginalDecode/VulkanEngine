#include <cstdio>
#include "gtest/gtest.h"

#include "Core/math/Vector4.h"
#include "Core/math/Vector3.h"
#include "Core/math/Vector2.h"
#include "Core/containers/GrowingArray.h"

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

TEST(Vector4, Length)
{
	constexpr float a = 193.f, b = 284.f, c = 321.f, d = 461.f;
	constexpr float constexpr_result = (a * a) + (b * b) + (c * c) + (d * d);
	Core::Vector4<float> vector(a, b, c, d);
	ASSERT_EQ(vector.Length(), sqrtf(constexpr_result));
}

TEST(Vector4, Length2)
{
	constexpr float a = 193.f, b = 284.f, c = 321.f, d = 461.f;
	constexpr float constexpr_result = (a * a) + (b * b) + (c * c) + (d * d);
	Core::Vector4<float> vector(a, b, c, d);
	ASSERT_EQ(vector.Length2(), constexpr_result);
}

TEST(Vector4, dot)
{
	constexpr float a = 193.f, b = 284.f, c = 321.f, d = 461.f;
	constexpr float e = 913.f, f = 378.f, g = 245.f, h = 560.f;
	constexpr float dot_result = (a * e) + (b * f) + (c * g) + (d * h);

	Core::Vector4<float> first(a, b, c, d);
	Core::Vector4<float> second(e, f, g, h);

	ASSERT_EQ(Core::Dot(first, second), dot_result);
}

TEST(GrowingArray, create)
{
	Core::GrowingArray<float> array;
	ASSERT_TRUE(array.Size() != array.Capacity());
	ASSERT_TRUE(array.Size() == 0);
	ASSERT_TRUE(array.Capacity() == 10); // if the default capacity ever changes this test will break
}

TEST(GrowingArray, addOne)
{
	Core::GrowingArray<float> array;
	array.Add(1.f);
	ASSERT_TRUE(array.Size() == 1);
}

TEST(GrowingArray, fill)
{
	Core::GrowingArray<float> array;
	for(int i = 0; i < array.Capacity(); i++)
		array.Add(1.f);

	ASSERT_EQ(array.Size(), array.Capacity());
}

TEST(GrowingArray, Copy)
{
	Core::GrowingArray<float> array;
	for(int i = 0; i < array.Capacity(); i++)
		array.Add(1.f);

	Core::GrowingArray<float> array2(array);

	// float* data = *(float**)( ( (char*)&array ) + ( sizeof( uint32 ) * 2 ) );
	// float* data2 = *(float**)( ( (char*)&array2 ) + ( sizeof( uint32 ) * 2 ) );

	float* data = &array[0];
	float* data2 = &array2[0];

	ASSERT_TRUE(data != nullptr);
	ASSERT_TRUE(data2 != nullptr);
	ASSERT_NE(data, data2);
}

TEST(GrowingArray, InitList) 
{
	Core::GrowingArray<int> array ({1,2,3,4,5,6,7,8,9,10});
	ASSERT_EQ(array.Size(), 10);
	ASSERT_EQ(array.Capacity(), 10);
	ASSERT_EQ(array.Size(), array.Capacity());
	int i = 1;
	for(int x : array)
	{
		ASSERT_EQ(x , i);
		i++;
	}
}

TEST(GrowingArray, Grow) 
{
	Core::GrowingArray<int> array({1,2,3,4,5,6,7,8,9,10});
	array.Add(11);
}


GTEST_API_ int main(int argc, char** argv)
{
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
