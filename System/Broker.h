#ifndef _VIX_META_BROKER_BROKER_H_
#define _VIX_META_BROKER_BROKER_H_

#include <Common/Thread.h>
#include <Network/VixMetaClient.h>
#include <Network/PluxityMetaClient.h>
#include <Setup/Setup.h>

#include <memory>
#include <list>

namespace VixMetaBroker
{
	class Broker : public Thread
	{
	public :
		Broker(Setup* pSetup);
		virtual ~Broker();

	private :
		Setup* _pSetup;
		
		std::list<std::shared_ptr<VixMetaClient>> _vixMetaClients;
		PluxityMetaClient _pluxityMetaClient;


	public :
		RET_E initialize();
		
	protected :
		void looperFunc() override;
	};
}

#endif
