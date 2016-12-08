#include "gtest/gtest.h"
#include "looper.h"

class LooperTest : public ::testing::Test
{
public:
	LooperTest()
	{

	}

	virtual ~LooperTest()
	{

	}
};

void SumFunc(int& sum, int v)
{
	sum += v;
}

void WorkThread()
{
	int sum = 0;

	Looper::Prepare();

	Looper* loop = Looper::ThisLooper();
	loop->GetMessageQueue()->EnqueueTask(FROM_HERE, Bind(&SumFunc, sum, 10), std::chrono::seconds(30));
	loop->GetMessageQueue()->EnqueueTask(FROM_HERE, Looper::QuitClosure());

	Looper::Loop();
}

void WorkThreadUI()
{
	int sum = 0;

	Looper::Prepare(Looper::kLoopUI);

	Looper* loop = Looper::ThisLooper();

	loop->GetMessageQueue()->EnqueueTask(FROM_HERE, Bind(&SumFunc, sum, 10), std::chrono::seconds(5));

	loop->GetMessageQueue()->EnqueueTask(FROM_HERE, Looper::QuitWhenIdleClosure());
	Looper::Loop();
}

TEST_F(LooperTest, MainTest)
{
// 	std::thread t(&WorkThread);
// 	t.join();
}

TEST_F(LooperTest, MainTest2)
{
// 	std::thread t(&WorkThreadUI);
// 	t.join();
}