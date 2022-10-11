#include <System/Broker.h>
#include <Common/Logger.h>

namespace VixMetaBroker
{
	Broker::Broker(Setup* pSetup = nullptr)
		: _pSetup(pSetup)
	{
		_vixMetaClients.clear();
	}

	Broker::~Broker()
	{
		if (_vixMetaClients.size()) {
			std::list<std::shared_ptr<VixMetaClient>>::iterator itr = _vixMetaClients.begin();
			for (; itr != _vixMetaClients.end(); itr++) {
				itr->get()->quit();
				itr->get()->join();
				_vixMetaClients.pop_front();
			}
		}

		_pluxityMetaClient.quit();
		_pluxityMetaClient.join();

		quit();
		join();
	}

	RET_E Broker::initialize()
	{
		VIXFATAL("Broker is Initialize Success...");

		if (_pSetup == nullptr) {
			VIXERROR("Setup is nullptr..");
			return VIX_FAILURE;
		}

		int32 vixServerCount = _pSetup->vixServerCount();
		if (vixServerCount == 0) {
			VIXERROR("Server informations is empty.");
			return VIX_FAILURE;
		}

		for (int32 i = 0; i < _pSetup->vixServerCount(); i++) {
			VixServerInfo* pInfo = _pSetup->getServerInfoByIdx(i);
			
			std::shared_ptr<VixMetaClient> pClient = std::make_shared<VixMetaClient>();
			_vixMetaClients.push_back(pClient);
			pClient->initialize(pInfo);
			
		}

		/* Pluxity Client */
		RET_E eRet = _pluxityMetaClient.initialize(&_pSetup->pluxityServerInfo());
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		runThread();

		return VIX_SUCCESS;
	}

	void Broker::looperFunc()
	{
		// Queueing for Pluxity
		static std::string lastMsg = "";
		std::list<std::shared_ptr<VixMetaClient>>::iterator itr = _vixMetaClients.begin();
		for (; itr != _vixMetaClients.end(); itr++) {
			std::string msg = (*itr)->getFrontMetaDataForPluxity();
			if (!msg.empty()) {
				if (msg == lastMsg) {
					continue;
				}
				else {
					_pluxityMetaClient.pushMsg(msg);
					lastMsg = msg;
					(*itr)->removeFrontMetaData();
				}
				
			}
		}


		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}