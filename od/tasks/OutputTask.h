#ifndef APP_TASKS_OUTPUTTASK_H_
#define APP_TASKS_OUTPUTTASK_H_

#include <od/tasks/Task.h>
#include <od/objects/Inlet.h>
#include <od/objects/Outlet.h>

namespace od
{

	class OutputTask : public Task
	{
	public:
		OutputTask();
		virtual ~OutputTask();

		virtual void process(float *inputs, float *outputs);

		Inlet mOut1{"OUT1"};
		Inlet mOut2{"OUT2"};
		Inlet mOut3{"OUT3"};
		Inlet mOut4{"OUT4"};

		Outlet mMonitor1{"Monitor1"};
		Outlet mMonitor2{"Monitor2"};
		Outlet mMonitor3{"Monitor3"};
		Outlet mMonitor4{"Monitor4"};
	};

} /* namespace od */

#endif /* APP_TASKS_OUTPUTTASK_H_ */
