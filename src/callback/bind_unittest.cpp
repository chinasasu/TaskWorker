#include "gtest/gtest.h"
#include "callback/bind.h"
#include "base/string_helper.h"
#include <iostream>

class BindTest : public ::testing::Test 
{
public:
	BindTest() 
	{

	}

	virtual ~BindTest() {
	}

// 	static void VoidFunc0(void) {
// 		static_func_mock_ptr->VoidMethod0();
// 	}
// 
// 	static int IntFunc0(void) { return static_func_mock_ptr->IntMethod0(); }
};

int Sum(int a, int b, int c, int d, int e, int f)
{
	return a + b + c + d + e + f;
}


TEST_F(BindTest, ArityTest)
{
	Callback<int(void)> cb0 = Bind(&Sum, 1, 2 , 3 , 4, 5, 6);
	EXPECT_EQ(21, cb0.Run());

	Callback<int(int)> cb1 = Bind(&Sum, 1, 2, 3, 4, 5);
	EXPECT_EQ(21, cb1.Run(6));

	Callback<int(int, int)> cb2 = Bind(&Sum, 1, 2, 3, 4);
	EXPECT_EQ(21, cb2.Run(5, 6));

	Callback<int(int, int, int)> cb3 = Bind(&Sum, 1, 2, 3);
	EXPECT_EQ(21, cb3.Run(4, 5, 6));

	Callback<int(int, int, int, int)> cb4 = Bind(&Sum, 1, 2);
	EXPECT_EQ(21, cb4.Run(3, 4, 5, 6));

	Callback<int(int, int, int, int, int)> cb5 = Bind(&Sum, 1);
	EXPECT_EQ(21, cb5.Run(2, 3, 4, 5, 6));

	//Callback<int(int, int, int, int, int, int)> cb6 = Bind(&Sum);
	//EXPECT_EQ(21, cb6.Run(1, 2, 3, 4, 5, 6));
}

TEST_F(BindTest, CurryingTest)
{
	
	Callback<int(int, int, int, int, int)> c5 = Bind(&Sum, 32);
	EXPECT_EQ(87, c5.Run(13, 12, 11, 10, 9));

// 	Callback<int(int, int, int, int)> c4 = Bind(c5, 16);
// 	EXPECT_EQ(94, c4.Run(13, 12, 11, 10));
}