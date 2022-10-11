#ifndef _VIX_META_BROKER_THREAD_H_
#define _VIX_META_BROKER_THREAD_H_

#include <Common/Type.h>

#include <thread>
#include <atomic>

namespace VixMetaBroker
{
	class Thread
	{
	public :
		Thread();
		virtual ~Thread();

	private :
		std::thread _thread;
		std::atomic<bool> _quitRequest;

	protected :
		bool _isRun;

	public :
		void quit() { _quitRequest = true; }
		bool isQuitRequested() const noexcept { return _quitRequest; }
		void runThread();

	public :
		static void coreThreadFunc(void* param);
		void threadFunc();

	protected :
		virtual void subLooperFunc() { }
		virtual void looperFunc() = 0;

	public :
		void join() { if (_thread.joinable()) _thread.join(); }
	};
}

#endif
