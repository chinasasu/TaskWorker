#include "gtest/gtest.h"
#include "message_queue_ui.h"

// class MessageQueueUIThreadTest : public ::testing::Test
// {
// public:
// 	MessageQueueUIThreadTest()
// 	{
// 
// 	}
// 
// 	virtual ~MessageQueueUIThreadTest()
// 	{
// 
// 	}
// };
// 
// void SumFunc2(int& sum, int v)
// {
// 	sum += v;
// 	//::Sleep(1000 * 15);
// }
// 
// void WorkThread(MessageQueueForUI* queue_ui)
// {
// 	//::Sleep(1000 * 5);
// 	int sum = 2;
// 	//queue_ui->EnqueueTask(FROM_HERE, Bind(&SumFunc2, sum, 10), std::chrono::seconds(5));
// 	//queue_ui->EnqueueTask(FROM_HERE, Bind(&SumFunc2, sum, 10));
// 	//queue_ui->EnqueueTask(FROM_HERE, Bind(&SumFunc2, sum, 30), std::chrono::seconds(5));
// 	//::Sleep(1000 * 5);
// 	queue_ui->EnqueueTask(FROM_HERE, Bind(&SumFunc2, sum, 11));
// 	queue_ui->EnqueueTask(FROM_HERE, Bind(&SumFunc2, sum, 12));
// 	//queue_ui->PostQuit();
// 
// }
// 
// TEST_F(MessageQueueUIThreadTest, MainTest)
// {
// 	int sum = 0;
// 
// 	MessageQueueForUI queueUI(true);
// 	
// 
// 	std::thread t(&WorkThread, &queueUI);
// 
// 	//std::thread t(&WorkThread2, &queueUI);
// 
// 	while (true)
// 	{
// 		PendingTask pendingTask = queueUI.Next();
// 		if (!pendingTask.IsValid())
// 			return;
// 
// 		pendingTask.task.Run();
// 
// 		//queueUI.Next();
// 	}
// 	
// 	t.join();
// 	// 
// 	// 	workerThread.Start();
// }