#ifndef __THREAD_H__
#define __THREAD_H__

#include <thread>
#include <mutex>

namespace common
{
   class Thread
   {
   public:
      Thread();
      virtual ~Thread();

   public:
      void Start();
      void Stop();

   protected:
      virtual void Run() = 0;

   protected:
      std::thread _thread;
      bool _is_run;
   };
}
#endif
