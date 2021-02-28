#include <hal/concurrency/Mutex.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

namespace od
{

	Mutex::Mutex()
	{
		Semaphore_Params params;
		Semaphore_Params_init(&params);
		params.mode = Semaphore_Mode_BINARY;
		mHandle = (void *)Semaphore_create(1, &params, NULL);
	}

	Mutex::~Mutex()
	{
		Semaphore_Handle h = (Semaphore_Handle)mHandle;
		Semaphore_delete(&h);
	}

	void Mutex::enter()
	{
		Semaphore_Handle h = (Semaphore_Handle)mHandle;
		Semaphore_pend(h, BIOS_WAIT_FOREVER);
	}

	void Mutex::leave()
	{
		Semaphore_Handle h = (Semaphore_Handle)mHandle;
		Semaphore_post(h);
	}

	bool Mutex::tryEnter()
	{
		Semaphore_Handle h = (Semaphore_Handle)mHandle;
		return Semaphore_pend(h, BIOS_NO_WAIT);
	}

  bool Mutex::tryEnter(unsigned int timeout)
  {
    Semaphore_Handle h = (Semaphore_Handle)mHandle;
    return Semaphore_pend(h, timeout);
  }

} /* namespace od */
