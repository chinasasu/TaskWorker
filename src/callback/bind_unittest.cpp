#include "gtest/gtest.h"
#include "callback/bind.h"
#include "base/string_helper.h"
#include <iostream>
#include "notification/notification_service.h"

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



static int SumValue = 0;

void Sum(int a, int b, int* outSumValue)
{
    *outSumValue = a + b;
}

class Foo
{
public:
	Foo() 
	{

	}

	~Foo()
	{

	}

// 	std::future<int> FuncWithWeak()
// 	{
// 		std::shared_ptr<Foo> ptr_foo(this);
// 		std::weak_ptr<Foo> weak_foo(ptr_foo);
// 
// 		return std::async(&Foo::DoTest, this);
// 	}

	int DoTest()
	{
		in_ = 10;
		// do something
		return 1;
	}

	void DoSome()
	{
		//std::cout << "None!" << endl;
	}

// 	std::weak_ptr<Foo> GetWeakPtr()
// 	{
// 		return weak_factory_.GetWeakPtr();
// 	}

private:
	//WeakPtrFactory<Foo> weak_factory_;
	int in_;
};

TEST_F(BindTest, ArityTest)
{
	int sum = 0;
	Closure cb0 = Bind(&Sum, 32, 16, &sum);
	cb0.Run();

	EXPECT_EQ(48, sum);

	std::string ss = "";
	std::string aa = StringPrintf("%d, %s", 0, u8"");


	NotificationService* service = NotificationService::current();
	
	//NotificationService::Notify()
}