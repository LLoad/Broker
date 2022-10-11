#ifndef _PLUXITY_META_CLIENT_H_
#define _PLUXITY_META_CLIENT_H_

#include <Network/HttpClient.h>
#include <Setup/PluxityServerInfo.h>

#include <atomic>
#include <deque>
#include <mutex>

namespace VixMetaBroker
{
	class PluxityHttpClient : public Client
	{
	public :
		PluxityHttpClient();
		virtual ~PluxityHttpClient();

	private :
		std::string _apiKey;
		uint32 _sendedCnt;

	public :
		const std::string& apiKey() const noexcept { return _apiKey; }

	protected :
		int32 callbackClientAppedHandshakeHeader(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		int32 callbackClientRcvHttp(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		int32 callbackClientHttpClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		
	};

	class PluxityMetaClient : public Thread
	{
	public :
		PluxityMetaClient();
		virtual ~PluxityMetaClient();

	public :
		enum class STATUS_E
		{
			READY = 0,
			SENDING_META = 1,
			SENDED_META = 2,
		};

	private :
		PluxityServerInfo* _pServerInfo;
		PluxityHttpClient _pluxityRestClient;

		std::atomic<STATUS_E> _eStatus;

		std::mutex _sendQueLock;
		std::deque<std::string> _sendMsgQue;

	public :
		RET_E initialize(PluxityServerInfo* pServerInfo);
		void pushMsg(std::string msg);
		void popFrontMsg();
	
	protected :
		void looperFunc() override;
		void setStatus(STATUS_E eStatus) { _eStatus = eStatus; }
		bool isStatusOn(STATUS_E eStatus) { return _eStatus == eStatus ? true : false; }

	};
}

#endif
