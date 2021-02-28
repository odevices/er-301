#pragma once

#include <hal/concurrency/Mutex.h>
#include <vector>

namespace od
{

	class Task;
	class TaskScheduler
	{
	public:
		TaskScheduler();
		virtual ~TaskScheduler();

		void process(float *inputs, float *outputs);
		void add(Task *task);
		void remove(Task *task);
		void beginTransaction();
		void endTransaction();

	private:
		std::vector<Task *> mTasks;
		std::vector<std::pair<int, Task *>> mTransactions;
		Mutex mMutex;
		int mTransactionDepth = 0;
	};

} /* namespace od */
