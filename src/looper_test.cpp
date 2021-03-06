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

	Looper::Prepare(Looper::kLoopUI);

	Looper* loop = Looper::ThisLooper();
	loop->GetMessageQueue()->EnqueueTask(FROM_HERE, Bind(&SumFunc, sum, 10), std::chrono::seconds(30));

	Looper::LoopUntilIdle();
}

void WorkThreadUI()
{
	int sum = 0;

	Looper::Prepare(Looper::kLoopUI);

	Looper* loop = Looper::ThisLooper();

	loop->GetMessageQueue()->EnqueueTask(FROM_HERE, Bind(&SumFunc, sum, 10), std::chrono::seconds(5));
	Looper::LoopUntilIdle();
}

TEST_F(LooperTest, MainTest)
{
	int sum = 0;

	Looper::Prepare();

	Looper* loop = Looper::ThisLooper();

	loop->task_runner()->PostDelayedTask(
		FROM_HERE,
		Bind(&SumFunc, sum, 10), std::chrono::seconds(30));

	Looper::LoopUntilIdle();
}

TEST_F(LooperTest, MainTest2)
{
	int sum = 0;

	Looper::Prepare();

	Looper* loop = Looper::ThisLooper();

	loop->task_runner()->PostTask(
		FROM_HERE,
		Bind(&SumFunc, sum, 10));

	Looper::LoopUntilIdle();
}