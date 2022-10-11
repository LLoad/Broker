#include <Common/Util.h>

#include <chrono>

namespace VixMetaBroker
{
	namespace TickUtil
	{
		uint64 currentTick()
		{
			using namespace std::chrono;
			return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
		}
	}
}