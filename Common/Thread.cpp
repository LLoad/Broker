#include <Common/Thread.h>

namespace VixMetaBroker
{
	Thread::Thread()
		: _isRun(false), _quitRequest(false)
	{
		
	}

	Thread::~Thread()
	{
		_isRun = false;
	}

	void Thread::runThread()
	{
		_isRun = true;
		_thread = std::thread(coreThreadFunc, this);
	}

	void Thread::coreThreadFunc(void* param)
	{
		reinterpret_cast<Thread*>(param)->threadFunc();
		return;
	}

	void Thread::threadFunc()
	{
		while (!isQuitRequested()) {
			looperFunc();
		}
	}
}