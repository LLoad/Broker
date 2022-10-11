#ifndef _VIX_META_BROKER_VIX_META_CLIENT_H_
#define _VIX_META_BROKER_VIX_META_CLIENT_H_

#include <Network/HttpClient.h>
#include <System/VixMetaParser.h>
#include <Network/VixMetaOption.h>
#include <Setup/VixServerInfo.h>

#include <atomic>

namespace VixMetaBroker
{
	class VixMetaClient : public Thread
	{
	public :
		VixMetaClient();
		virtual ~VixMetaClient();

	public:
		enum class STATUS_E
		{
			READY = 0,
			REST_TRY_LOGIN = 1,
			INIT_WS_CLIENT,
			REQUEST_METADATA,

			REQUEST_TERMINATE,
		};

	private :
		VixServerInfo* _pServerInfo;

		HttpClient _vixRestClient;
		WsClient _vixWsClient;
		MetaOption _vixMetaOption;
		VixMetaParser _vixMetaParser;

		std::atomic<STATUS_E> _eStatus;

		uint32 _sendedToPluxityCnt;
		std::string _lastSendedMsg;
		

	public :
		RET_E initialize(VixServerInfo* pServerInfo);
		void requestMetaData(const MetaOption& metaOpt);
		std::string getFrontMetaDataForPluxity();
		void removeFrontMetaData();

	protected :
		void looperFunc() override;
		void setStatus(STATUS_E eStatus) { _eStatus = eStatus; }
		bool isStatusOn(STATUS_E eStatus) { return _eStatus == eStatus ? true : false; }

	private :
		RET_E initRestClient();
		RET_E initWsClient();

		NET_MSG_S makeLoginMsg();
		NET_MSG_S makeRequestMetaMsg();
		NET_MSG_S makeKeepAliveMsg();

		void checkLoginProc();
		void checkClientProc();
		void checkKeepAliveProc();
		void checkVixParserQue();

		void closeClient();
	};
}

#endif
