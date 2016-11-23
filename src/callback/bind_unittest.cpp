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

class Foo
{
public:
	int DoSum(int a, int b, int c ) { return a + b; };
};

int Sum(int a, int b, int c, int d, int e, int f)
{
	return a + b + c + d + e + f;
}

template <typename T>
void VoidPolymorphic1(T t) {
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

	Callback<int(int, int, int, int, int, int)> cb6 = Bind(&Sum);
	EXPECT_EQ(21, cb6.Run(1, 2, 3, 4, 5, 6));
}

TEST_F(BindTest, BindWithPurePtr)
{
	Foo foo;

	Callback<int(void)> cb0 = Bind(&Foo::DoSum, &foo, 1, 2, 3);
	EXPECT_EQ(6, cb0.Run());

	Callback<int(int)> cb1 = Bind(&Foo::DoSum, &foo, 1, 2);
	EXPECT_EQ(6, cb1.Run(3));

	Callback<int(int,int)> cb2 = Bind(&Foo::DoSum, &foo, 1);
	EXPECT_EQ(6, cb2.Run(2,3));
}

TEST_F(BindTest, BindWithSharedPtr)
{
	std::shared_ptr<Foo> shared_foo(new Foo);

	Callback<int(void)> cb0 = Bind(&Foo::DoSum, shared_foo, 1, 2, 3);
	EXPECT_EQ(6, cb0.Run());

	Callback<int(int)> cb1 = Bind(&Foo::DoSum, shared_foo, 1, 2);
	EXPECT_EQ(6, cb1.Run(3));

	Callback<int(int, int)> cb2 = Bind(&Foo::DoSum, shared_foo, 1);
	EXPECT_EQ(6, cb2.Run(2, 3));
}

TEST_F(BindTest, BindWithWeakPtr)
{
	std::shared_ptr<Foo> shared_foo(new Foo);
	std::weak_ptr<Foo> weak_foo = shared_foo;
	
	Callback<int(void)> cb0 = Bind(&Foo::DoSum, weak_foo, 1, 2, 3);
	EXPECT_EQ(6, cb0.Run());

	Callback<int(int)> cb1 = Bind(&Foo::DoSum, weak_foo, 1, 2);
	EXPECT_EQ(6, cb1.Run(3));

	Callback<int(int, int)> cb2 = Bind(&Foo::DoSum, weak_foo, 1);
	EXPECT_EQ(6, cb2.Run(2, 3));
}

TEST_F(BindTest, BindWithLamba)
{
	Callback<int(void)> cb0 = Bind([] { return 0; });
	EXPECT_EQ(0, cb0.Run());

	Callback<int(int)> cb1 = Bind([](int a) { return a; });
	EXPECT_EQ(1, cb1.Run(1));

	Callback<int(int,int)> cb2 = Bind([](int a, int b) { return a + b; });
	EXPECT_EQ(3, cb2.Run(1, 2));
}

TEST_F(BindTest, CurryingTest)
{
	Callback<int(int, int, int, int, int)> c5 = Bind(&Sum, 32);
	EXPECT_EQ(87, c5.Run(13, 12, 11, 10, 9));

	Callback<void(const int&)> cb = Bind(VoidPolymorphic1<const int&>);

 	//auto c4 = Bind(c5,1,1,2,1,1);

// 	auto tt = BindDemo(&Sum);
// 
// 	tt.Run();
// 	EXPECT_EQ(94, c4.Run(13, 12, 11, 10));
}