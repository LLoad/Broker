#include <Network/Client.h>

#include <Network/VixMetaClient.h>
#include <Common/Logger.h>
#include <Network/VixMetaOption.h>

#include <System/Broker.h>

#if defined(_WIN32)

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // !_WINSOCKAPI_

#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "Psapi")
#pragma comment(lib, "Userenv")
#endif

#include <deque>
#include <vector>

using namespace VixMetaBroker;

int32 main()
{
	VIXFATAL("VixMetaBroker Start...");

	Setup setup;
	RET_E eRet = setup.load();
	if (eRet != VIX_SUCCESS) {
		VIXFATAL("Setup load failed...");
	}

	Broker broker(&setup);
	broker.initialize();

	broker.join();

	return 0;
}
