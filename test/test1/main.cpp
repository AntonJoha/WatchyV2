#include <unity.h>


void test()
{
	TEST_ASSERT_TRUE(1 == 2-1);
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test);
	UNITY_END();
}



